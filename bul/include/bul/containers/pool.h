#pragma once

#include <iterator>

#include "bul/bul.h"
#include "bul/containers/buffer.h"
#include "bul/containers/handle.h"

namespace bul
{
template <typename T>
class Pool
{
private:
    struct Data
    {
        uint32_t full : 1;
        uint32_t version : 31;
        union
        {
            T value;
            uint32_t next;
        };
    };

public:
    Pool()
        : Pool(1)
    {}

    explicit Pool(size_t capacity)
    {
        data_.resize(capacity);
        free_ = 0;
        for (uint32_t i = 0; i < data_.size(); ++i)
        {
            data_[i].full = 0;
            data_[i].version = 0;
            data_[i].next = i + 1;
        }
        data_[data_.size() - 1].next = UINT32_MAX;
    }

    ~Pool()
    {
        clear();
    }

    Pool(const Pool<T>&) = delete;
    Pool<T>& operator=(const Pool<T>&) = delete;

    Pool(Pool<T>&& other)
    {
        *this = std::move(other);
    }

    Pool<T>& operator=(Pool<T>&& other)
    {
        data_ = std::move(other.data_);
        free_ = other.free_;
        return *this;
    }

    Handle<T> insert(T&& val)
    {
        Data* free_data = nullptr;
        uint32_t free_index = get_next_free_slot(free_data);
        memset(&free_data->value, 0, sizeof(T));
        free_data->value = std::move(val);
        return Handle<T>{free_index, free_data->version};
    }

    void erase(Handle<T> handle)
    {
        ASSERT(is_valid(handle));
        uint32_t free_index = free_;
        free_ = handle.value;
        Data& data = data_[free_];
        if constexpr (!std::is_trivially_destructible_v<T>)
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

    void clear()
    {
        free_ = 0;
        for (size_t i = 0; i < data_.size(); ++i)
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                if (data_[i].full)
                {
                    data_[i].value.~T();
                }
            }
            data_[i].full = 0;
            data_[i].version = 0;
            data_[i].next = i + 1;
        }
        data_[data_.size() - 1].next = UINT32_MAX;
    }

    struct Iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        Iterator(size_t index, Buffer<Data>& data)
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
        Buffer<Data>& data_;
    };

    Iterator begin()
    {
        size_t i = 0;
        for (; i < data_.size(); ++i)
        {
            if (data_[i].full)
            {
                break;
            }
        }
        return Iterator{i, data_};
    }

    Iterator end()
    {
        return Iterator{data_.size(), data_};
    }

private:
    uint32_t get_next_free_slot(Data*& free_data)
    {
        if (free_ == UINT32_MAX)
        {
            size_t current_size = data_.size();
            data_.resize(current_size * 2);
            free_ = current_size;
            for (uint32_t i = current_size; i < data_.size(); ++i)
            {
                data_[i].full = 0;
                data_[i].version = 0;
                data_[i].next = i + 1;
            }
            data_[data_.size() - 1].next = UINT32_MAX;
        }

        free_data = &data_[free_];
        uint32_t free_index = free_;
        free_ = free_data->next;
        free_data->full = 1;
        return free_index;
    }

    Buffer<Data> data_;
    uint32_t free_ = UINT32_MAX;
};
} // namespace bul
