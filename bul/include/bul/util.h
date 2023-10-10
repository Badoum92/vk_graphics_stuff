#pragma once

#include <string_view>
#include <type_traits>

namespace bul
{
std::wstring to_wstring(const std::string_view str);
std::string from_wstring(const std::wstring_view wstr);

template <typename E>
requires std::is_enum_v<E>
std::underlying_type_t<E> to_underlying(E e)
{
    return std::underlying_type_t<E>(e);
}

template<typename E, typename T>
requires std::is_convertible_v<T, std::underlying_type_t<E>>
E to_enum(T t)
{
    return E(t);
}
} // namespace bul
