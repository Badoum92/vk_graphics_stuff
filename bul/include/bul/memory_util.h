#pragma once

#include <cstring>
#include <type_traits>

namespace bul
{
template <typename T>
void copy_range(const T* first, const T* last, T* dst)
{
    if constexpr (std::is_trivially_copyable_v<T>)
    {
        memmove(dst, first, (last - first) * sizeof(T));
    }
    else
    {
        for (; first != last; ++dst, ++first)
        {
            *dst = *first;
        }
    }
}

template <typename T>
void uninitialized_copy_range(const T* first, const T* last, T* dst)
{
    if constexpr (std::is_trivially_copyable_v<T>)
    {
        memmove(dst, first, (last - first) * sizeof(T));
    }
    else
    {
        for (; first != last; ++dst, ++first)
        {
            new (dst) T(*first);
        }
    }
}

template <typename T>
void move_range(T* first, T* last, T* dst)
{
    if constexpr (std::is_trivially_copyable_v<T>)
    {
        memmove(dst, first, (last - first) * sizeof(T));
    }
    else
    {
        for (; first != last; ++dst, ++first)
        {
            *dst = std::move(*first);
        }
    }
}

template <typename T>
void uninitialized_move_range(T* first, T* last, T* dst)
{
    if constexpr (std::is_trivially_copyable_v<T>)
    {
        memmove(dst, first, (last - first) * sizeof(T));
    }
    else
    {
        for (; first != last; ++dst, ++first)
        {
            new (dst) T(std::move(*first));
        }
    }
}

template <typename T>
void default_construct_range(T* first, T* last)
{
    for (; first != last; ++first)
    {
        new (first) T();
    }
}

template <typename T>
void delete_range(T* first, T* last)
{
    if constexpr (!std::is_trivially_destructible_v<T>)
    {
        for (; first != last; ++first)
        {
            first->~T();
        }
    }
}
} // namespace bul