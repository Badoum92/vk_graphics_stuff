#pragma once

#include <type_traits>
#include <initializer_list>

namespace bul
{
template <typename T, size_t CAPACITY>
class StaticVector
{
    static_assert(std::is_default_constructible_v<T>, "Type must be default constructible");

public:
    constexpr StaticVector() = default;

    constexpr StaticVector(std::initializer_list<T> vals)
    {
        for (const auto& val : vals)
        {
            push_back(val);
        }
    }

    constexpr ~StaticVector()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = 0; i < size_; ++i)
            {
                data_[i].~T();
            }
        }
    }

    constexpr StaticVector(const StaticVector&) = default;
    constexpr StaticVector& operator=(const StaticVector&) = default;
    constexpr StaticVector(StaticVector&&) = default;
    constexpr StaticVector& operator=(StaticVector&&) = default;

    constexpr bool resize(size_t new_size)
    {
        if (new_size > CAPACITY)
        {
            return false;
        }
        for (size_t i = size_; i < new_size; ++i)
        {
            new (data_ + i) T();
        }
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = new_size; i < size_; ++i)
            {
                data_[i].~T();
            }
        }
        size_ = new_size;
        return true;
    }

    constexpr bool push_back(const T& val)
    {
        return emplace_back(val);
    }

    constexpr bool push_back(T&& val)
    {
        return emplace_back(std::move(val));
    }

    template <typename... Args>
    constexpr bool emplace_back(Args&&... args)
    {
        if (size_ == CAPACITY)
        {
            return false;
        }
        new (data_ + size_) T(std::forward<Args>(args)...);
        ++size_;
        return true;
    }

    constexpr bool pop_back()
    {
        if (size_ == 0)
        {
            return false;
        }
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            data_[size_ - 1].~T();
        }
        --size_;
        return true;
    }

    constexpr void clear()
    {
        while (pop_back())
        {
            continue;
        }
    }

    constexpr size_t capacity() const
    {
        return CAPACITY;
    }

    constexpr size_t size() const
    {
        return size_;
    }

    constexpr bool empty() const
    {
        return size() == 0;
    }

    constexpr T* begin()
    {
        return data_;
    }

    constexpr T* end()
    {
        return data_ + size_;
    }

    constexpr const T* data() const
    {
        return data_;
    }

    constexpr T* data()
    {
        return data_;
    }

    constexpr const T& back() const
    {
        return data_[size_ - 1];
    }

    constexpr T& back()
    {
        return data_[size_ - 1];
    }

    constexpr const T& operator[](size_t index) const
    {
        return data_[index];
    }

    constexpr T& operator[](size_t index)
    {
        return data_[index];
    }

private:
    size_t size_ = 0;
    alignas(T) T data_[CAPACITY] = {};
};
} // namespace bul
