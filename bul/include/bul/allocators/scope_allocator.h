#pragma once

#include "bul/types.h"

namespace bul
{
struct scope_allocator
{
    using free_t = void (*)(void*);

    explicit scope_allocator(size_t capacity);
    scope_allocator(void* buffer, size_t size, free_t free_function = nullptr);
    ~scope_allocator();

    scope_allocator(const scope_allocator& other);
    scope_allocator& operator=(const scope_allocator& other);

    scope_allocator(scope_allocator&& other);
    scope_allocator& operator=(scope_allocator&& other);

    void* allocate(size_t size, size_t alignment = 8);

    uint8_t* begin = nullptr;
    uint8_t* end = nullptr;
    uint8_t* previous = nullptr;
    uint8_t* current = nullptr;
    free_t free_function = nullptr;
};
} // namespace bul
