#pragma once

#include <stdlib.h>
#include <iterator>
#include <type_traits>

#include "bul/bul.h"
#include "bul/containers/handle.h"

namespace bul
{
template <typename T>
struct pool
{
    static_assert(std::is_move_constructible_v<T>, "Type must be move constructible");

    struct data
    {
        uint32_t free : 1;
        uint32_t version : 31;
        union
        {
            T value;
            uint32_t next_free;
        };
    };

    pool()
        : pool(1)
    {}

    explicit pool(uint32_t capacity_)
    {
        _data = reinterpret_cast<data*>(malloc(capacity_ * sizeof(data)));
        _capacity = capacity_;
        _first_free = 0;
        for (uint32_t i = 0; i < _capacity; ++i)
        {
            _data[i].free = 1;
            _data[i].version = 0;
            _data[i].next_free = i + 1;
        }
        _data[_capacity - 1].next_free = UINT32_MAX;
    }

    ~pool()
    {
        clear();
        free(_data);
        _data = nullptr;
        _capacity = 0;
        _first_free = UINT32_MAX;
    }

    pool(pool<T>&& other)
    {
        clear();
        free(_data);
        _data = other._data;
        _capacity = other._capacity;
        _first_free = other._first_free;
        other._data = nullptr;
        other._capacity = 0;
        other._first_free = UINT32_MAX;
    }

    pool<T>& operator=(pool<T>&& other)
    {
        clear();
        free(_data);
        _data = other._data;
        _capacity = other._capacity;
        _first_free = other._first_free;
        other._data = nullptr;
        other._capacity = 0;
        other._first_free = UINT32_MAX;
        return *this;
    }

    template <typename... Args>
    handle<T> insert(Args&&... args)
    {
        uint32_t free_slot = get_next_free_slot();
        data& data = _data[free_slot];
        _first_free = data.next_free;
        data.free = 0;
        new (&data.value) T(std::forward<Args>(args)...);
        return handle<T>{free_slot, data.version};
    }

    void erase(handle<T> handle)
    {
        ASSERT(is_valid(handle));
        uint32_t next_free = _first_free;
        _first_free = handle.value;
        data& data = _data[handle.value];
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            data.value.~T();
        }
        data.next_free = next_free;
        ++data.version;
        data.free = 1;
    }

    bool is_valid(handle<T> handle) const
    {
        return handle.value < _capacity && handle.version == _data[handle.value].version && !_data[handle.value].free;
    }

    const T& get(handle<T> handle) const
    {
        ASSERT(is_valid(handle));
        return _data[handle.value].value;
    }

    T& get(handle<T> handle)
    {
        ASSERT(is_valid(handle));
        return _data[handle.value].value;
    }

    void clear()
    {
        _first_free = 0;
        for (uint32_t i = 0; i < _capacity; ++i)
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
            {
                if (!_data[i].free)
                {
                    _data[i].value.~T();
                }
            }
            _data[i].free = 1;
            _data[i].version = 0;
            _data[i].next_free = i + 1;
        }
        _data[_capacity - 1].next_free = UINT32_MAX;
    }

    struct iterator
    {
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        iterator(uint32_t index, uint32_t capacity, data* data)
            : _index(index)
            , _capacity(capacity)
            , _data(data)
        {}

        reference operator*() const
        {
            return _data[_index].value;
        }

        pointer operator->()
        {
            return &_data[_index].value;
        }

        iterator& operator++()
        {
            if (_index == _capacity)
            {
                return *this;
            }
            do
            {
                ++_index;
            } while (_index < _capacity && _data[_index].free);
            return *this;
        }

        iterator operator++(int)
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend bool operator==(const iterator& a, const iterator& b)
        {
            return a._data == b._data && a._index == b._index;
        }

        friend bool operator!=(const iterator& a, const iterator& b)
        {
            return !(a == b);
        }

        uint32_t _index = 0;
        uint32_t _capacity = 0;
        data* _data = nullptr;
    };

    iterator begin()
    {
        uint32_t index = 0;
        while (index < _capacity && _data[index].free)
        {
            ++index;
        }
        return iterator{index, _capacity, _data};
    }

    iterator end()
    {
        return iterator{_capacity, _capacity, _data};
    }

    uint32_t get_next_free_slot()
    {
        if (_first_free == UINT32_MAX)
        {
            uint32_t current_capacity = _capacity;
            uint32_t new_capacity = _capacity * 2;
            data* new_data = reinterpret_cast<data*>(malloc(new_capacity * sizeof(data)));

            for (uint32_t i = 0; i < _capacity; ++i)
            {
                new_data[i].free = 0;
                new_data[i].version = _data[i].version;
                new_data[i].value = std::move(_data[i].value);
            }

            for (uint32_t i = _capacity; i < new_capacity; ++i)
            {
                new_data[i].free = 1;
                new_data[i].version = 0;
                new_data[i].next_free = i + 1;
            }
            new_data[new_capacity - 1].next_free = UINT32_MAX;

            free(_data);
            _data = new_data;
            _capacity = new_capacity;
            _first_free = current_capacity;
        }
        return _first_free;
    }

    data* _data = nullptr;
    uint32_t _capacity = 0;
    uint32_t _first_free = UINT32_MAX;
};

//template <typename T>
//struct apool
//{
//    handle<T> insert(const T& element)
//    {
//        if (size == capacity)
//        {
//            _grow();
//        }
//        handle_index& first_free_handle_index = handle_indices[first_free];
//        uint32_t next_free = first_free_handle_index.index;
//        data[size] = element;
//        first_free_handle_index.index = size;
//        handles[size] = first_free;
//        handle<T> handle = {first_free, first_free_handle_index.version};
//        first_free = next_free;
//        return handle;
//    }
//
//    void remove(handle<T> handle)
//    {}
//
//    const T* get(handle<T> handle) const
//    {
//        return data[handle_indices[handle.value].index];
//    }
//
//    T* get(handle<T> handle)
//    {
//        return data[handle_indices[handle.value].index];
//    }
//
//    const T* begin() const
//    {
//        return data;
//    }
//
//    T* begin()
//    {
//        return data;
//    }
//
//    const T* end() const
//    {
//        return data + size;
//    }
//
//    T* end()
//    {
//        return data + size;
//    }
//
//    void _grow()
//    {
//        uint32_t new_capacity = capacity * 2 + (capacity == 0);
//        T* new_data = (T*)malloc(new_capacity * sizeof(T));
//        uint32_t* new_handles = (uint32_t*)malloc(new_capacity * sizeof(uint32_t));
//        handle_index* new_handle_indices = (handle_index*)malloc(new_capacity * sizeof(handle_index));
//        memcpy(new_data, data, size * sizeof(T));
//        memcpy(new_handles, handles, size * sizeof(uint32_t));
//        memcpy(new_handle_indices, handle_indices, size * sizeof(handle_index));
//        free(data);
//        free(handles);
//        free(handle_indices);
//        capacity = new_capacity;
//        data = new_data;
//        handles = new_handles;
//        handle_indices = new_handle_indices;
//        for (uint32_t i = size; i < capacity; ++i)
//        {
//            handle_indices[i] = {i + 1, 0};
//        }
//        handle_indices[capacity - 1].index = UINT32_MAX;
//        first_free_handle_index = size;
//    }
//
//    struct handle_index
//    {
//        uint32_t index;
//        uint32_t generation;
//    };
//
//    T* data = nullptr;
//    uint32_t* handles = nullptr;
//    handle_index* handle_indices = nullptr;
//    uint32_t size = 0;
//    uint32_t capacity = 0;
//    uint32_t first_free = UINT32_MAX;
//};
} // namespace bul
