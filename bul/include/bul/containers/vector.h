#pragma once

#include <stdlib.h>
#include <string.h>

#include "bul/bul.h"
#include "bul/containers/span.h"

namespace bul
{
template <typename T>
struct vector
{
    void destroy()
    {
        free(data);
        data = nullptr;
        size = 0;
        capacity = 0;
    }

    void reserve(uint32_t capacity_)
    {
        if (capacity_ <= capacity)
        {
            return;
        }

        capacity = capacity_;
        T* new_data = (T*)malloc(capacity * sizeof(T));
        memcpy(new_data, data, size * sizeof(T));
        free(data);
        data = new_data;
    }

    void resize(uint32_t size_)
    {
        if (size_ > capacity)
        {
            reserve(size_);
        }
        size = size_;
    }

    T& push_back(const T& value)
    {
        T& element = push_back();
        element = value;
        return element;
    }

    T& push_back()
    {
        if (size == capacity)
        {
            capacity = capacity * 2 + (capacity == 0);
            T* new_data = (T*)malloc(capacity * sizeof(T));
            memcpy(new_data, data, size * sizeof(T));
            free(data);
            data = new_data;
        }
        T& element = data[size++];
        return element;
    }

    T pop_back()
    {
        ASSERT(size > 0);
        T element = data[--size];
        return element;
    }

    void clear()
    {
        size = 0;
    }

    uint32_t size_bytes() const
    {
        return size * sizeof(T);
    }

    T& operator[](uint32_t index)
    {
        ASSERT(index < size);
        return data[index];
    }

    const T& operator[](uint32_t index) const
    {
        ASSERT(index < size);
        return data[index];
    }

    T& front()
    {
        ASSERT(size > 0);
        return data[0];
    }

    const T& front() const
    {
        ASSERT(size > 0);
        return data[0];
    }

    T& back()
    {
        ASSERT(size > 0);
        return data[size - 1];
    }

    const T& back() const
    {
        ASSERT(size > 0);
        return data[size - 1];
    }

    T* begin()
    {
        return data;
    }

    const T* begin() const
    {
        return data;
    }

    T* end()
    {
        return data + size;
    }

    const T* end() const
    {
        return data + size;
    }

    operator span<T>()
    {
        return span<T>{data, size};
    }

    operator span<const T>() const
    {
        return span<const T>{data, size};
    }

    T* data = nullptr;
    uint32_t size = 0;
    uint32_t capacity = 0;
};
} // namespace bul
