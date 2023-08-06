#pragma once

#include <type_traits>

#include "bul/bul.h"
#include "bul/containers/buffer.h"

namespace bul
{
template <typename T>
class Vector
{
public:
    Vector() = default;

    Vector(size_t size)
    {
        resize(size);
    }

    Vector(const Vector<T>& other)
    {
        *this = other;
    }

    Vector<T>& operator=(const Vector<T>& other)
    {
        Buffer<T> new_buffer(other.size_);
        for (size_t i = 0; i < other.size(); ++i)
        {
            new_buffer[i] = other[i];
        }
        buffer_ = std::move(new_buffer);
        size_ = other.size();
    }

    Vector(Vector<T>&&) = default;
    Vector<T>& operator=(Vector<T>&&) = default;

    void resize(size_t size)
    {
        if (size < size_)
        {
            shrink(size);
            return;
        }

        reserve(size);
        for (size_t i = size_; i < size; ++i)
        {
            new (&buffer_[i]) T();
        }
        size_ = size;
    }

    void reserve(size_t size)
    {
        if (size < capacity())
        {
            return;
        }

        size_t new_capacity = capacity() == 0 ? 2 : capacity() * 2;
        Buffer<T> new_buffer(new_capacity);
        for (size_t i = 0; i < buffer_.size(); ++i)
        {
            if constexpr (std::is_move_constructible_v<T>)
            {
                new_buffer[i] = std::move(buffer_[i]);
            }
            else
            {
                new_buffer[i] = buffer_[i];
            }
        }
        buffer_ = std::move(new_buffer);
    }

    T& push_back(const T& value)
    {
        return emplace_back(value);
    }

    T& push_back(T&& value)
    {
        return emplace_back(std::move(value));
    }

    template <typename... Args>
    T& emplace_back(Args&&... args)
    {
        if (size() == capacity())
        {
            reserve(capacity() == 0 ? 2 : capacity() * 2);
        }
        T* ret = new (&buffer_[size_]) T(std::forward<Args>(args)...);
        ++size_;
        return *ret;
    }

    T&& pop_back()
    {
        --size_;
        return std::move(buffer_[size_]);
    }

    void clear()
    {
        shrink(0);
    }

    size_t size() const
    {
        return size_;
    }

    size_t capacity() const
    {
        return buffer_.size();
    }

    bool empty() const
    {
        return size_ == 0;
    }

    T* data()
    {
        return buffer_.data();
    }

    const T* data() const
    {
        return buffer_.data();
    }

    T& operator[](size_t i)
    {
        return buffer_[i];
    }

    const T& operator[](size_t i) const
    {
        return buffer_[i];
    }

    T& front()
    {
        return buffer_[0];
    }

    const T& front() const
    {
        return buffer_[0];
    }

    T& back()
    {
        return buffer_[size_ - 1];
    }

    const T& back() const
    {
        return buffer_[size_ - 1];
    }

    T* begin()
    {
        return &buffer_[0];
    }

    T* end()
    {
        return begin() + size_;
    }

private:
    Buffer<T> buffer_;
    size_t size_ = 0;

    void shrink(size_t size)
    {
        if (size >= size_)
        {
            return;
        }

        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = size; i < size_; ++i)
            {
                buffer_[i].~T();
            }
        }
        size_ = size;
    }
};
} // namespace bul
