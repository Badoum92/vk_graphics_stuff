#pragma once

#include "core/core.h"

struct linear_allocator
{
    uint8_t* begin;
    uint8_t* end;
    uint8_t* current;
};

linear_allocator linear_allocator_create(uint32_t size);
void linear_allocator_destroy(linear_allocator* allocator);
linear_allocator* linear_allocator_get_global();
void linear_allocator_set_global(linear_allocator* allocator);

void* linear_alloc(linear_allocator* allocator, uint32_t size);
void* linear_alloc_aligned(linear_allocator* allocator, uint32_t size, uint32_t alignment = sizeof(void*) * 2);
void linear_free(linear_allocator* allocator, void* ptr);
void linear_reset(linear_allocator* allocator);
void linear_reset_globals();
