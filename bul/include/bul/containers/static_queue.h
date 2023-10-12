#pragma once

#include <type_traits>

#include "bul/bul.h"
#include "bul/memory_util.h"

namespace bul
{
template <typename T, size_t CAPACITY>
struct static_queue
{
    constexpr static_queue() = default;

    constexpr ~static_queue()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            for (size_t i = _begin; i != _end; i = (i + 1) % CAPACITY)
            {
                _data[i].~T();
            }
        }
    }

    constexpr static_queue(const static_queue<T, CAPACITY>& other)
    {
        *this = other;
    }

    constexpr static_queue<T, CAPACITY>& operator=(const static_queue<T, CAPACITY>& other)
    {
        clear();
        _begin = 0;
        _end = other._size;
        _size = other._size;
        if (other._begin < other._end)
        {
            uninitialized_copy_range(other._data + other._begin, other._data + other._end, _data);
        }
        else
        {
            uninitialized_copy_range(other._data + other._begin, other._data + CAPACITY, _data);
            uninitialized_copy_range(other._data, other._data + other._end, _data + CAPACITY - other._begin);
        }
    }

    constexpr static_queue(static_queue<T, CAPACITY>&& other)
    {
        *this = std::move(other);
    }

    constexpr static_queue<T, CAPACITY>& operator=(static_queue<T, CAPACITY>&& other)
    {
        clear();
        _begin = 0;
        _end = other._size;
        _size = other._size;
        if (other._begin < other._end)
        {
            uninitialized_copy_range(other._data + other._begin, other._data + other._end, _data);
        }
        else
        {
            uninitialized_copy_range(other._data + other._begin, other._data + CAPACITY, _data);
            uninitialized_copy_range(other._data, other._data + other._end, _data + CAPACITY - other._begin);
        }
    }

    template <typename... Args>
    constexpr T* emplace(Args&&... args)
    {
        if (_size == CAPACITY)
        {
            return nullptr;
        }
        T* ret = new (_data + _end) T(std::forward<Args>(args)...);
        _end = (_end + 1) % CAPACITY;
        ++_size;
        return ret;
    }

    constexpr bool pop()
    {
        if (_size == 0)
        {
            return false;
        }
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            _data[_begin].~T();
        }
        _begin = (_begin + 1) % CAPACITY;
        --_size;
        return true;
    }

    constexpr size_t capacity() const
    {
        return CAPACITY;
    }

    constexpr size_t size() const
    {
        return _size;
    }

    constexpr bool empty() const
    {
        return _size == 0;
    }

    constexpr const T& front() const
    {
        ASSERT(_size > 0);
        return _data[_begin];
    }

    constexpr T& front()
    {
        ASSERT(_size > 0);
        return _data[_begin];
    }

    constexpr void clear()
    {
        while (_size > 0)
        {
            pop();
        }
    }

    size_t _begin = 0;
    size_t _end = 0;
    size_t _size = 0;
    alignas(T) T _data[CAPACITY] = {};
};
} // namespace bul
