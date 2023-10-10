#pragma once

#include <cstdlib>
#include <type_traits>
#include <initializer_list>

#include "bul/bul.h"
#include "bul/containers/span.h"
#include "bul/memory_util.h"

namespace bul
{
template <typename T>
struct vector
{
    static_assert(std::is_default_constructible_v<T>, "Type must be default constructible");
    static_assert(std::is_move_constructible_v<T>, "Type must be move constructible");

    vector() = default;

    explicit vector(size_t size)
    {
        resize(size);
    }

    vector(std::initializer_list<T> init_list)
    {
        reserve(init_list.size());
        _current = _end;
        copy_range(init_list.begin(), init_list.end(), _begin);
    }

    vector(size_t size, const T& default_value)
    {
        reserve(size);
        _current = _end;
        for (T* cur = _begin; cur != _end; ++cur)
        {
            *cur = default_value;
        }
    }

    ~vector()
    {
        clear();
        free(_begin);
        _begin = nullptr;
        _current = nullptr;
        _end = nullptr;
    }

    vector(const vector<T>& other)
    {
        clear();
        free(_begin);
        _begin = reinterpret_cast<T*>(malloc(other.size_bytes()));
        _current = _begin + other.size();
        _end = _current;
        copy_range(other._begin, other._end, _begin);
    }

    vector<T>& operator=(const vector<T>& other)
    {
        clear();
        free(_begin);
        _begin = reinterpret_cast<T*>(malloc(other.size_bytes()));
        _current = _begin + other.size();
        _end = _current;
        copy_range(other._begin, other._end, _begin);
        return *this;
    }

    vector(vector<T>&& other) noexcept
    {
        *this = std::move(other);
    }

    vector<T>& operator=(vector<T>&& other) noexcept
    {
        free(_begin);
        _begin = other._begin;
        _current = other._current;
        _end = other._end;
        other._begin = nullptr;
        other._current = nullptr;
        other._end = nullptr;
        return *this;
    }

    void resize(size_t size_)
    {
        if (size_ < size())
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                delete_range(_begin + size_, _current);
            }
        }
        else
        {
            reserve(size_);
            default_construct_range(_current, _end);
        }
        _current = _begin + size_;
    }

    void reserve(size_t size_)
    {
        if (size_ <= capacity())
        {
            return;
        }

        T* new_begin = reinterpret_cast<T*>(malloc(size_ * sizeof(T)));
        T* new_end = new_begin + size_;
        T* new_current = new_begin + size();
        move_range(_begin, _current, new_begin);
        free(_begin);
        _begin = new_begin;
        _current = new_current;
        _end = new_end;
    }

    template <typename... Args>
    T& emplace_back(Args&&... args)
    {
        if (_current == _end)
        {
            reserve(capacity() == 0 ? 1 : capacity() * 2);
        }
        T* ret = new (_current) T(std::forward<Args>(args)...);
        ++_current;
        return *ret;
    }

    T&& pop_back()
    {
        ASSERT(_current != _begin);
        --_current;
        return std::move(*_current);
    }

    void clear()
    {
        resize(0);
    }

    size_t find(const T& to_find) const
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

    void erase(size_t index)
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

    void erase_stable(size_t index)
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

    size_t size() const
    {
        return _current - _begin;
    }

    size_t size_bytes() const
    {
        return size() * sizeof(T);
    }

    size_t capacity() const
    {
        return _end - _begin;
    }

    bool empty() const
    {
        return _current == _begin;
    }

    T* data()
    {
        return _begin;
    }

    const T* data() const
    {
        return _begin;
    }

    T& operator[](size_t index)
    {
        ASSERT(index < size());
        return _begin[index];
    }

    const T& operator[](size_t index) const
    {
        ASSERT(index < size());
        return _begin[index];
    }

    T& front()
    {
        ASSERT(size() > 0);
        return *_begin;
    }

    const T& front() const
    {
        ASSERT(size() > 0);
        return *_begin;
    }

    T& back()
    {
        ASSERT(size() > 0);
        return *(_current - 1);
    }

    const T& back() const
    {
        ASSERT(size() > 0);
        return *(_current - 1);
    }

    T* begin()
    {
        return _begin;
    }

    const T* begin() const
    {
        return _begin;
    }

    T* end()
    {
        return _end;
    }

    const T* end() const
    {
        return _end;
    }

    operator span<T>()
    {
        return span<T>{_begin, _current};
    }

    operator span<const T>() const
    {
        return span<const T>{_begin, _current};
    }

    T* _begin = nullptr;
    T* _current = nullptr;
    T* _end = nullptr;
};
} // namespace bul
