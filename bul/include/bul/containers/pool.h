#pragma once

#include <vector>
#include <variant>

#include "bul/bul.h"
#include "bul/containers/handle.h"

namespace bul
{
template <typename T>
class Pool
{
private:
    struct Data
    {
        template <typename... Args>
        Data(Args&&... args)
            : full{1}
            , version{0}
            , value{std::forward<Args>(args)...}
        {}

        uint32_t full : 1;
        uint32_t version : 31;
        union
        {
            T value;
            uint32_t next;
        };
    };

public:
    explicit Pool(size_t capacity = 4)
    {
        data_.reserve(capacity);
    }

    Handle<T> insert(const T& val)
    {
        return emplace(val);
    }

    Handle<T> insert(T&& val)
    {
        return emplace(std::move(val));
    }

    template <typename... Args>
    Handle<T> emplace(Args&&... args)
    {
        if (free_ == UINT32_MAX)
        {
            data_.emplace_back(std::forward<Args>(args)...);
            return Handle<T>{(uint32_t)data_.size() - 1, 0};
        }

        Data& free_data = data_[free_];
        uint32_t free_index = free_;
        free_ = free_data.next;
        new (&free_data.value) T(std::forward<Args>(args)...);
        free_data.full = 1;
        return Handle<T>{free_index, free_data.version};
    }

    void erase(Handle<T> handle)
    {
        ASSERT(is_valid(handle));
        uint32_t free_index = free_;
        free_ = handle.value;
        Data& data = data_[free_];
        if (!std::is_trivially_destructible_v<T>)
        {
            data.value.~T();
        }
        data.next = free_index;
        ++data.version;
        data.full = 0;
    }

    bool is_valid(Handle<T> handle) const
    {
        return handle.value < data_.size() && handle.version == data_[handle.value].version
            && data_[handle.value].full == 1;
    }

    const T& get(Handle<T> handle) const
    {
        ASSERT(is_valid(handle));
        return data_[handle.value].value;
    }

    T& get(Handle<T> handle)
    {
        ASSERT(is_valid(handle));
        return data_[handle.value].value;
    }

    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        Iterator(size_t index, std::vector<Data>& data)
            : index_(index)
            , data_(data)
        {}

        reference operator*() const
        {
            return data_[index_].value;
        }

        pointer operator->()
        {
            return &data_[index_].value;
        }

        Iterator& operator++()
        {
            ++index_;
            while (index_ < data_.size() && data_[index_].full == 0)
            {
                ++index_;
            }
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const Iterator& a, const Iterator& b)
        {
            return a.data_.data() == b.data_.data() && a.index_ == b.index_;
        }

        friend bool operator!=(const Iterator& a, const Iterator& b)
        {
            return !(a == b);
        }

    private:
        size_t index_;
        std::vector<Data>& data_;
    };

    Iterator begin()
    {
        return Iterator{0, data_};
    }

    Iterator end()
    {
        return Iterator{data_.size(), data_};
    }

private:
    std::vector<Data> data_;
    uint32_t free_ = UINT32_MAX;
};
} // namespace bul
