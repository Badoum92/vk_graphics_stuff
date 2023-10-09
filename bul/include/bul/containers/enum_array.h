#pragma once

#include <type_traits>

#include "bul/bul.h"
#include "bul/util.h"

namespace bul
{
template <typename Enum>
concept CountEnum = requires(Enum e)
{
    std::is_enum_v<Enum>;
    Enum::Count;
};

template <CountEnum Enum, typename T>
struct EnumArray
{
    static constexpr size_t SIZE = size_t(Enum::Count);

    constexpr T& operator[](Enum e)
    {
        ASSERT(to_underlying(e) < SIZE);
        return data[to_underlying(e)];
    }

    constexpr const T& operator[](Enum e) const
    {
        ASSERT(to_underlying(e) < SIZE);
        return data[to_underlying(e)];
    }

    T data[SIZE];
};
} // namespace bul
