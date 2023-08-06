#pragma once

#include "bul/bul.h"

#include <type_traits>

namespace bul
{
template <typename Enum>
    requires std::is_enum_v<Enum>
constexpr auto operator+(Enum e)
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}

template <typename Enum>
concept CountEnum = requires(Enum e)
{
    std::is_enum_v<Enum>;
    Enum::Count;
};

template <CountEnum Enum, typename T>
struct EnumArray
{
    static constexpr size_t SIZE = static_cast<size_t>(Enum::Count);

    constexpr T& operator[](Enum e)
    {
        ASSERT(+e < SIZE);
        return data[+e];
    }

    constexpr const T& operator[](Enum e) const
    {
        ASSERT(+e < SIZE);
        return data[+e];
    }

    T data[SIZE];
};
} // namespace bul
