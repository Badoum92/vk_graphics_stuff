#pragma once

#include "bul/bul.h"

namespace bul
{
template <typename T>
struct span
{
    span(T* _data, uint32_t _size)
        : data(_data)
        , size(_size)
    {}

    template <uint32_t SIZE>
    span(T (&&array)[SIZE])
        : data(array)
        , size(SIZE)
    {}

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

    template <typename U>
    constexpr span<U> as() const
    {
        return span<U>{reinterpret_cast<U*>(data), size_bytes() / sizeof(U)};
    }

    constexpr operator span<const T>() const
    {
        return span<const T>{data, size};
    }

    T* data;
    uint32_t size;
};
} // namespace bul
