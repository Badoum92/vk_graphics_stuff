#pragma once

#include <string_view>
#include <concepts>

namespace bul
{
#ifdef _WIN32
std::wstring to_utf16(const std::string_view str);
std::string from_utf16(const std::wstring_view wstr);
#endif

template <typename E>
requires std::is_enum_v<E>
std::underlying_type_t<E> to_underlying(E e)
{
    return static_cast<std::underlying_type_t<E>>(e);
}

template<typename E, typename T>
requires std::is_convertible_v<T, std::underlying_type_t<E>>
E to_enum(T t)
{
    return static_cast<E>(t);
}
} // namespace bul
