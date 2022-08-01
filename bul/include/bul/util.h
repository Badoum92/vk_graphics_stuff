#pragma once

#include <string_view>
#include <type_traits>

namespace bul
{
#ifdef _WIN32
std::wstring to_utf16(const std::string_view str);
std::string from_utf16(const std::wstring_view wstr);
#endif

template <typename E>
std::underlying_type_t<E> to_underlying(E e)
{
    return static_cast<std::underlying_type_t<E>>(e);
}

template<typename E, typename T>
E to_enum(T t)
{
    static_assert(std::is_convertible_v<T, std::underlying_type_t<E>>);
    return static_cast<E>(t);
}
} // namespace bul
