#pragma once

#include <type_traits>

template <typename Enum>
    requires std::is_enum_v<Enum>
auto operator+(Enum e)
{
    return static_cast<std::underlying_type_t<Enum>>(e);
}
