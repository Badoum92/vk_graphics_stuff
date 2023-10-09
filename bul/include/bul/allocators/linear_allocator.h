#pragma once

#include "bul/types.h"

namespace bul
{
struct linear_allocator : public non_copyable
{
    using free_t = void (*)(void*);

    explicit linear_allocator(size_t capacity);
    linear_allocator(void* buffer, size_t size, free_t free_function = nullptr);
    ~linear_allocator();

    linear_allocator(linear_allocator&& other);
    linear_allocator& operator=(linear_allocator&& other);

    void* allocate(size_t size, size_t alignment = 8);

    template <typename T>
    T* allocate()
    {
        return allocate(sizeof(T), alignof(T));
    }

    void free(void* ptr)
    {
        current = ptr;
    }

    void clear()
    {
        current = begin;
    }

    uint8_t* begin = nullptr;
    uint8_t* end = nullptr;
    uint8_t* current = nullptr;
    free_t free_function = nullptr;
};
} // namespace bul
