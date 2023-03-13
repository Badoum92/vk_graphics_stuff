#pragma once

#include "bul/bul.h"

#include <cstdlib>

namespace bul
{
template <typename T>
class Buffer
{
public:
    Buffer() = default;

    Buffer(size_t size)
    {
        data_ = (T*)calloc(1, size * sizeof(T));
        ASSERT(data_ != nullptr);
        size_ = size;
    }

    Buffer(const Buffer<T>&) = delete;
    Buffer<T>& operator=(const Buffer<T>&) = delete;

    Buffer(Buffer<T>&& other)
    {
        *this = std::move(other);
    }

    Buffer<T>& operator=(Buffer<T>&& other)
    {
        free(data_);
        data_ = other.data_;
        size_ = other.size_;
        other.data_ = nullptr;
        other.size_ = 0;
        return *this;
    }

    ~Buffer()
    {
        free(data_);
        data_ = nullptr;
        size_ = 0;
    }

    void resize(size_t size)
    {
        data_ = (T*)realloc(data_, size * sizeof(T));
        ASSERT(data_ != nullptr);
        if (size > size_)
        {
            memset(data_ + size_, 0, (size - size_) * sizeof(T));
        }
        size_ = size;
    }

    size_t size() const
    {
        return size_;
    }

    size_t size_bytes() const
    {
        return size_ * sizeof(T);
    }

    const T* data() const
    {
        return data_;
    }

    T* data()
    {
        return data_;
    }

    T& operator[](size_t i)
    {
        return data_[i];
    }

    const T& operator[](size_t i) const
    {
        return data_[i];
    }

private:
    T* data_ = nullptr;
    size_t size_ = 0;
};
} // namespace bul
