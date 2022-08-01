#pragma once

#include <vector>

#include "bul/containers/handle.h"

namespace bul
{
template <typename T, size_t SIZE = 8>
class Pool
{
public:
    Pool()
    {
        data_.reserve(SIZE);
        handles_.reserve(SIZE);
    }

    Handle<T> insert(T&& val)
    {
        if (free_.value == UINT32_MAX)
        {
            data_.push_back(std::forward<T>(val));
            return handles_.emplace_back(handles_.size());
        }

        uint32_t free_index = free_.value;
        free_.value = handles_[free_index].value;
        handles_[free_index].value = free_index;
        data_[free_index] = std::forward<T>(val);
        return handles_[free_index];
    }

    template<typename... Args>
    Handle<T> emplace(Args&&... args)
    {
        if (free_.value == UINT32_MAX)
        {
            data_.emplace_back(std::forward(args...));
            return handles_.emplace_back(handles_.size());
        }

        uint32_t free_index = free_.value;
        free_.value = handles_[free_index].value;
        handles_[free_index].value = free_index;
        new (&data_[free_index]) T(args...);
        return handles_[free_index];
    }

    bool remove(Handle<T> handle)
    {
        if (!handle.is_valid() || handles_[handle.value].version != handle.version)
        {
            return false;
        }

        if (!std::is_trivially_destructible_v<T>)
        {
            data_[handle.value].~T();
        }

        uint32_t free_index = free_.value;
        free_.value = handle.value;
        handles_[handle.value].value = free_index;
        handles_[handle.value].version++;
        return true;
    }

    bool is_valid(Handle<T> handle) const
    {
        return handle.is_valid() && handles_[handle.value].version == handle.version;
    }

    const T& get(Handle<T> handle) const
    {
        ASSERT(is_valid(handle));
        return data_[handle.value];
    }

    T& get(Handle<T> handle)
    {
        ASSERT(is_valid(handle));
        return data_[handle.value];
    }

    const std::vector<Handle<T>>& handles() const
    {
        return handles_;
    }

private:
    std::vector<T> data_;
    std::vector<Handle<T>> handles_;
    Handle<T> free_ = {};
};
} // namespace bul
