#pragma once

#include "bul/bul.h"

namespace bul
{
template <typename E, typename T>
struct enum_array
{
    static constexpr uint32_t SIZE = (uint32_t)E::_count;

    constexpr T& operator[](E e)
    {
        ASSERT((uint32_t)e < SIZE);
        return data[(uint32_t)e];
    }

    constexpr const T& operator[](E e) const
    {
        ASSERT((uint32_t)e < SIZE);
        return data[(uint32_t)e];
    }

    T data[SIZE];
};
} // namespace bul
