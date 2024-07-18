#pragma once

#include "bul/bul.h"
#include "bul/allocators/linear_allocator.h"

namespace bul
{
struct scope_allocator
{
    static scope_allocator create(linear_allocator* linear_allocator);
    static scope_allocator create_global();

    ~scope_allocator();

    void* alloc(uint32_t size);
    void* alloc_aligned(uint32_t size, uint32_t alignment = sizeof(void*));

    linear_allocator* allocator;
    uint8_t* begin;
};
} // namespace bul
