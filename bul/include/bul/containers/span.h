#pragma once

#include "bul/bul.h"

namespace bul
{
template <typename T>
struct span
{
    constexpr span(T* begin, T* end)
        : _begin(begin)
        , _end(end)
    {}

    constexpr span(T* data, size_t size)
        : _begin(data)
        , _end(data + size)
    {}

    constexpr span(span<T> other, size_t offset)
    {
        ASSERT(offset <= other.size_);
        return span<T>(other._begin + offset, other.size_ - offset);
    }

    constexpr span(span<T> other, size_t offset, size_t size)
    {
        ASSERT(offset + size <= other.size_);
        return span<T>(other._begin + offset, size);
    }

    constexpr span(const span<T>& other)
    {
        *this = other;
    }

    constexpr span<T>& operator=(const span<T>& other)
    {
        _begin = other._begin;
        _end = other._end;
        return *this;
    }

    constexpr size_t size() const
    {
        return _end - _begin;
    }

    constexpr size_t size_bytes() const
    {
        return size() * sizeof(T);
    }

    constexpr T& operator[](size_t i)
    {
        ASSERT(i < size());
        return _begin[i];
    }

    constexpr const T& operator[](size_t i) const
    {
        ASSERT(i < size());
        return _begin[i];
    }

    constexpr T* begin()
    {
        return _begin;
    }

    constexpr T* end()
    {
        return _end;
    }

    constexpr const T* begin() const
    {
        return _begin;
    }

    constexpr const T* end() const
    {
        return _end;
    }

    constexpr T* data()
    {
        return _begin;
    }

    constexpr T* data() const
    {
        return _begin;
    }

    constexpr bool empty() const
    {
        return _begin == _end;
    }

    template <typename U>
    constexpr span<U> as() const
    {
        return span(reinterpret_cast<U*>(data), size_bytes() / sizeof(U));
    }

    constexpr operator span<const T>() const
    {
        return span<const T>{_begin, _end};
    }

    T* _begin = nullptr;
    T* _end = nullptr;
};
} // namespace bul
