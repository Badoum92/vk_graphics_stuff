#pragma once

#include <type_traits>

#include "bul/bul.h"
#include "bul/util.h"

namespace bul
{
template <typename E>
concept count_enum = requires(E e)
{
    std::is_enum_v<E>;
    E::_count;
};

template <count_enum E, typename T>
struct enum_array
{
    static constexpr size_t SIZE = size_t(E::_count);

    constexpr T& operator[](E e)
    {
        ASSERT(to_underlying(e) < SIZE);
        return data[to_underlying(e)];
    }

    constexpr const T& operator[](E e) const
    {
        ASSERT(to_underlying(e) < SIZE);
        return data[to_underlying(e)];
    }

    T data[SIZE];
};
} // namespace bul
