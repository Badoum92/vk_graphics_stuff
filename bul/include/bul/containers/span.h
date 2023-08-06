#pragma once

#include "bul/bul.h"

namespace bul
{
template <typename T>
class Span
{
public:
    Span(T* data, size_t size)
        : data_(data)
        , size_(size)
    {}

    Span(T* begin, T* end)
        : data_(begin)
        , size_(end - begin)
    {}

    Span(Span<T> other, size_t offset)
    {
        ASSERT(offset <= other.size_);
        return Span<T>(other.data_ + offset, other.size_ - offset);
    }

    Span(Span<T> other, size_t offset, size_t size)
    {
        ASSERT(offset + size <= other.size_);
        return Span<T>(other.data_ + offset, size);
    }

    size_t size() const
    {
        return size_;
    }

    size_t size_btyes() const
    {
        return size_ * sizeof(T);
    }

    T& operator[](size_t i) const
    {
        ASSERT(i < size_);
        return data[i];
    }

    T* begin() const
    {
        return data_;
    }

    T* end() const
    {
        return data_ + size_;
    }

    T* data() const
    {
        return data_;
    }

    bool empty() const
    {
        return size_ == 0;
    }

    template <typename U>
    Span<U> as() const
    {
        return Span((U*)data, size_bytes() / sizeof(U));
    }

private:
    T* data_ = nullptr;
    size_t size_ = 0;
};
} // namespace bul
