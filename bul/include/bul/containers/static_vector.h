#pragma once

#include <new>
#include <utility>
#include <type_traits>
#include <initializer_list>

#include "bul/bul.h"
#include "bul/containers/span.h"

namespace bul
{
template <typename T, size_t CAPACITY>
struct static_vector
{
    static_assert(std::is_default_constructible_v<T>, "Type must be default constructible");
    static_assert(std::is_move_constructible_v<T>, "Type must be move constructible");

    constexpr static_vector() = default;

    constexpr static_vector(std::initializer_list<T> init_list)
    {
        for (const auto& element : init_list)
        {
            emplace_back(element);
        }
    }

    constexpr ~static_vector()
    {
        clear();
    }

    constexpr static_vector(const static_vector<T, CAPACITY>& other)
    {
        *this = other;
    }

    constexpr static_vector& operator=(const static_vector<T, CAPACITY>& other)
    {
        for (size_t i = 0; i < _size; ++i)
        {
            new (&_data[i]) T(other._data[i]);
        }
        for (size_t i = other._size; i < _size; ++i)
        {
            _data[i].~T();
        }
        other._size = 0;
    }

    constexpr static_vector(static_vector<T, CAPACITY>&& other)
    {
        *this = std::move(other);
    }

    constexpr static_vector& operator=(static_vector<T, CAPACITY>&& other)
    {
        for (size_t i = 0; i < _size; ++i)
        {
            new (&_data[i]) T(std::move(other._data[i]));
        }
        for (size_t i = other._size; i < _size; ++i)
        {
            _data[i].~T();
        }
        other._size = 0;
    }

    constexpr void resize(size_t size_)
    {
        ASSERT(size_ <= CAPACITY)
        if (size_ < _size)
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                for (size_t i = size_; i < _size; ++i)
                {
                    _data[i].~T();
                }
            }
        }
        else
        {
            for (size_t i = _size; i < size_; ++i)
            {
                new (&_data[i]) T();
            }
        }
        _size = size_;
    }

    template <typename... Args>
    constexpr T& emplace_back(Args&&... args)
    {
        ASSERT(_size < CAPACITY);
        T* ret = new (&_data[_size]) T(std::forward<Args>(args)...);
        ++_size;
        return *ret;
    }

    constexpr T&& pop_back()
    {
        ASSERT(_size > 0);
        --_size;
        return std::move(_data[_size]);
    }

    constexpr void clear()
    {
        resize(0);
    }

    constexpr size_t capacity() const
    {
        return CAPACITY;
    }

    constexpr size_t size() const
    {
        return _size;
    }

    constexpr size_t size_bytes() const
    {
        return _size * sizeof(T);
    }

    constexpr bool empty() const
    {
        return _size == 0;
    }

    constexpr T* begin()
    {
        return _data;
    }

    constexpr const T* begin() const
    {
        return _data;
    }

    constexpr T* end()
    {
        return _data + _size;
    }

    constexpr const T* end() const
    {
        return _data + _size;
    }

    constexpr T* data()
    {
        return _data;
    }

    constexpr const T* data() const
    {
        return _data;
    }

    constexpr T& back()
    {
        return _data[_size - 1];
    }

    constexpr const T& back() const
    {
        return _data[_size - 1];
    }

    constexpr T& operator[](size_t index)
    {
        return _data[index];
    }

    constexpr const T& operator[](size_t index) const
    {
        return _data[index];
    }

    operator span<T>()
    {
        return span<T>{_data, _size};
    }

    operator span<const T>() const
    {
        return span<const T>{_data, _size};
    }

    size_t _size = 0;
    alignas(T) T _data[CAPACITY] = {};
};
} // namespace bul
