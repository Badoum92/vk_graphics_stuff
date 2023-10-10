#pragma once

#include <new>
#include <type_traits>
#include <initializer_list>

#include "bul/bul.h"
#include "bul/containers/span.h"
#include "bul/memory_util.h"

namespace bul
{
template <typename T, size_t CAPACITY>
struct static_vector
{
    static_assert(std::is_default_constructible_v<T>, "Type must be default constructible");
    static_assert(std::is_move_constructible_v<T>, "Type must be move constructible");

    constexpr static_vector() = default;

    constexpr explicit static_vector(const T& default_value)
    {
        _current = _end;
        for (T* cur = _begin; cur != _end; ++cur)
        {
            *cur = default_value;
        }
    }

    constexpr static_vector(std::initializer_list<T> init_list)
    {
        ASSERT(init_list.size() <= CAPACITY);
        _current = _end;
        copy_range(init_list.begin(), init_list.end(), _begin);
    }

    constexpr ~static_vector()
    {
        clear();
    }

    constexpr static_vector(const static_vector<T, CAPACITY>& other)
    {
        clear();
        _current = _begin + other.size();
        copy_range(other._begin, other._end, _begin);
    }

    constexpr static_vector& operator=(const static_vector<T, CAPACITY>& other)
    {
        clear();
        _current = _begin + other.size();
        copy_range(other._begin, other._end, _begin);
        return *this;
    }

    constexpr static_vector(static_vector<T, CAPACITY>&& other) noexcept
    {
        *this = std::move(other);
    }

    constexpr static_vector& operator=(static_vector<T, CAPACITY>&& other) noexcept
    {
        _current = _begin + other.size();
        move_range(other._begin, other._end, _begin);
        other._current = other._begin;
        return *this;
    }

    constexpr void resize(size_t size_)
    {
        ASSERT(size_ <= CAPACITY);
        if (size_ < size())
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                delete_range(_begin + size_, _current);
            }
        }
        else
        {
            default_construct_range(_current, _end);
        }
        _current = _begin + size_;
    }

    template <typename... Args>
    constexpr T& emplace_back(Args&&... args)
    {
        ASSERT(_current != _end);
        T* ret = new (_current) T(std::forward<Args>(args)...);
        ++_current;
        return *ret;
    }

    constexpr T&& pop_back()
    {
        ASSERT(_current != _begin);
        --_current;
        return std::move(*_current);
    }

    constexpr void clear()
    {
        resize(0);
    }

    constexpr size_t find(const T& to_find) const
    {
        for (const T* cur = _begin; cur != _current; ++cur)
        {
            if (*cur == to_find)
            {
                return cur - _begin;
            }
        }
        return size_t(-1);
    }

    constexpr void erase(size_t index)
    {
        if (index == size_t(-1))
        {
            return;
        }

        ASSERT(index < size());
        std::swap(_begin[index], back());
        --_current;
        if (!std::is_trivially_default_constructible_v<T>)
        {
            _current->~T();
        }
    }

    constexpr void erase_stable(size_t index)
    {
        if (index == size_t(-1))
        {
            return;
        }

        ASSERT(index < size());
        move_range(_begin + index + 1, _current, _begin + index);
        --_current;
        if (!std::is_trivially_default_constructible_v<T>)
        {
            _current->~T();
        }
    }

    constexpr size_t size() const
    {
        return _current - _begin;
    }

    constexpr size_t size_bytes() const
    {
        return size() * sizeof(T);
    }

    constexpr size_t capacity() const
    {
        return CAPACITY;
    }

    constexpr bool empty() const
    {
        return _current == _begin;
    }

    constexpr T* data()
    {
        return _begin;
    }

    constexpr const T* data() const
    {
        return _begin;
    }

    constexpr T& operator[](size_t index)
    {
        ASSERT(index < size());
        return _begin[index];
    }

    constexpr const T& operator[](size_t index) const
    {
        ASSERT(index < size());
        return _begin[index];
    }

    constexpr T& front()
    {
        ASSERT(size() > 0);
        return *_begin;
    }

    constexpr const T& front() const
    {
        ASSERT(size() > 0);
        return *_begin;
    }

    constexpr T& back()
    {
        ASSERT(size() > 0);
        return *(_current - 1);
    }

    constexpr const T& back() const
    {
        ASSERT(size() > 0);
        return *(_current - 1);
    }

    constexpr T* begin()
    {
        return _begin;
    }

    constexpr const T* begin() const
    {
        return _begin;
    }

    constexpr T* end()
    {
        return _end;
    }

    constexpr const T* end() const
    {
        return _end;
    }

    constexpr operator span<T>()
    {
        return span<T>{_begin, _current};
    }

    constexpr operator span<const T>() const
    {
        return span<const T>{_begin, _current};
    }

    alignas(T) T _begin[CAPACITY] = {};
    T* _current = _begin;
    T* _end = _begin + CAPACITY;
};
} // namespace bul
