#pragma once

#include "bul/bul.h"

namespace bul
{
struct linear_allocator
{
    static linear_allocator create(void* buffer, uint32_t size);

    void* alloc(uint32_t size);
    void* alloc_aligned(uint32_t size, uint32_t alignment = sizeof(void*));
    void reset(void* ptr);

    uint8_t* begin;
    uint8_t* end;
    uint8_t* current;
};

extern thread_local linear_allocator g_linear_allocator;
} // namespace bul
