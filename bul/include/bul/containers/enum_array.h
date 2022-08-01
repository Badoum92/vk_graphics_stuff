#pragma once

#include "bul/bul.h"

namespace bul
{
template <typename E>
concept CountEnum = requires(E e)
{
    E::Count;
};

template <CountEnum E, typename T>
struct EnumArray
{
    static constexpr size_t SIZE = static_cast<size_t>(E::Count);

    constexpr T& operator[](E e)
    {
        ASSERT(static_cast<size_t>(e) < SIZE);
        return data[static_cast<size_t>(e)];
    }

    constexpr const T& operator[](E e) const
    {
        ASSERT(static_cast<size_t>(e) < SIZE);
        return data[static_cast<size_t>(e)];
    }

    T data[SIZE];
};
} // namespace bul
