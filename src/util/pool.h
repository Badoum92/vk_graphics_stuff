#pragma once

#include <vector>

#include "handle.h"

template <typename T, uint32_t SIZE = 8>
class Pool
{
public:
    Pool()
    {
        data_.reserve(SIZE);
    }

    Handle<T> insert(const T& value)
    {
        uint32_t index = 0;
        Handle<T> handle = Handle<T>::invalid();
        if (free_.empty())
        {
            index = data_.size();
            data_.push_back(value);
            handle = Handle<T>{index};
            handles_.push_back(handle);
        }
        else
        {
            index = free_.back();
            free_.pop_back();
            data_[index] = value;
            handle = Handle<T>{index};
            handles_[index] = handle;
        }
        return handles_[index];
    }

    void remove(const Handle<T>& handle)
    {
        if (!handle.is_valid() || handles_[handle.value()] != handle)
        {
            return;
        }

        free_.push_back(handle.value());
        handles_[handle.value()] = Handle<T>::invalid();
    }

    const T& get(const Handle<T>& handle) const
    {
        assert(handle.is_valid() && handles_[handle.value()] == handle);
        return data_[handle.value()];
    }

    T& get(const Handle<T>& handle)
    {
        assert(handle.is_valid() && handles_[handle.value()] == handle);
        return data_[handle.value()];
    }

    std::vector<Handle<T>>& handles()
    {
        return handles_;
    }

private:
    std::vector<T> data_;
    std::vector<Handle<T>> handles_;
    std::vector<uint32_t> free_;
};
