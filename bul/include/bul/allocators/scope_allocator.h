#pragma once

#include "bul/bul.h"
#include "bul/allocators/linear_allocator.h"

namespace bul
{
struct scope_allocator
{
    struct dtor_list
    {
        void (*dtor)(void* ptr);
        dtor_list* next;
    };
    static_assert(sizeof(dtor_list) == 16);

    scope_allocator(linear_allocator allocator)
        : _allocator(allocator)
        , _scope_begin(allocator._current)
    {}

    ~scope_allocator()
    {
        for (dtor_list* dtor_list = _dtor_list; dtor_list; dtor_list = dtor_list->next)
        {
            dtor_list->dtor(dtor_list + 1);
        }
        _allocator.rewind(_scope_begin);
    }

    void* alloc(uint32_t size)
    {
        return _allocator.alloc(size);
    }

    void* alloc_aligned(uint32_t size, uint32_t alignment)
    {
        return _allocator.alloc(size, alignment);
    }

    template <typename T, typename... Args>
    T* alloc_object(Args&&... args)
    {
        dtor_list* dtor_list = (dtor_list*)alloc_aligned(sizeof(sizeof(dtor_list) + sizeof(T), alignof(dtor_list)));
        T* object = (T*)(dtor_list + 1);

        dtor_list->next = nullptr;
        dtor_list->dtor = [](void* ptr) { ((T*)ptr)->~T(); };

        _dtor_list->next = dtor_list;
        _dtor_list = dtor_list;

        return new (object)(std::forward<Args>(args)...);
    }

    linear_allocator _allocator;
    uint8_t* _scope_begin;
    dtor_list* _dtor_list;
};
} // namespace bul
