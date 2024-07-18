#pragma once

#include "bul/bul.h"
#include "bul/containers/span.h"

namespace bul
{
template <typename T, uint32_t CAPACITY>
struct static_vector
{
    constexpr void resize(uint32_t size_)
    {
        ASSERT(size_ <= CAPACITY);
        size = size_;
    }

    constexpr T& push_back(const T& value)
    {
        ASSERT(size < CAPACITY);
        T& element = push_back();
        element = value;
        return element;
    }

    constexpr T& push_back()
    {
        ASSERT(size < CAPACITY);
        T& element = data[size++];
        return element;
    }

    constexpr T pop_back()
    {
        ASSERT(size > 0);
        T element = data[--size];
        return element;
    }

    constexpr void clear()
    {
        size = 0;
    }

    constexpr uint32_t size_bytes() const
    {
        return size * sizeof(T);
    }

    constexpr T& operator[](uint32_t index)
    {
        ASSERT(index < size);
        return data[index];
    }

    constexpr const T& operator[](uint32_t index) const
    {
        ASSERT(index < size);
        return data[index];
    }

    constexpr T& front()
    {
        ASSERT(size > 0);
        return data[0];
    }

    constexpr const T& front() const
    {
        ASSERT(size > 0);
        return data[0];
    }

    constexpr T& back()
    {
        ASSERT(size > 0);
        return data[size - 1];
    }

    constexpr const T& back() const
    {
        ASSERT(size > 0);
        return data[size - 1];
    }

    constexpr T* begin()
    {
        return data;
    }

    constexpr const T* begin() const
    {
        return data;
    }

    constexpr T* end()
    {
        return data + size;
    }

    constexpr const T* end() const
    {
        return data + size;
    }

    constexpr operator span<T>()
    {
        return span<T>{data, size};
    }

    constexpr operator span<const T>() const
    {
        return span<const T>{data, size};
    }

    T data[CAPACITY];
    uint32_t size = 0;
    static constexpr uint32_t capacity = CAPACITY;
};
} // namespace bul
