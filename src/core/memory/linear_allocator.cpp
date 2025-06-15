#include "core/memory/linear_allocator.h"

#include <stdlib.h>

#include "core/thread.h"

static thread_local linear_allocator* global_linear_allocator = nullptr;
static linear_allocator* global_linear_allocators[16] = {nullptr};

linear_allocator linear_allocator_create(uint32_t size)
{
    linear_allocator allocator;
    allocator.begin = (uint8_t*)malloc(size);
    allocator.current = allocator.begin;
    allocator.end = allocator.begin + size;
    return allocator;
}

void linear_allocator_destroy(linear_allocator* allocator)
{
    free(allocator->begin);
    allocator->begin = nullptr;
    allocator->current = nullptr;
    allocator->end = nullptr;
    if (global_linear_allocator == allocator)
    {
        global_linear_allocator = nullptr;
    }
}

linear_allocator* linear_allocator_get_global()
{
    return global_linear_allocator;
}

void linear_allocator_set_global(linear_allocator* allocator)
{
    global_linear_allocator = allocator;
    global_linear_allocators[thread_get_id()] = allocator;
}

void* linear_alloc(linear_allocator* allocator, uint32_t size)
{
    uint8_t* ptr = allocator->current;
    allocator->current += size;
    ASSERT(allocator->current <= allocator->end);
    return ptr;
}

void* linear_alloc_aligned(linear_allocator* allocator, uint32_t size, uint32_t alignment)
{
    allocator->current = (uint8_t*)align((void*)allocator->current, alignment);
    return linear_alloc(allocator, size);
}

void linear_free(linear_allocator* allocator, void* ptr)
{
    ASSERT(allocator->begin <= ptr && ptr <= allocator->end);
    allocator->current = (uint8_t*)ptr;
}

void linear_reset(linear_allocator* allocator)
{
    allocator->current = allocator->begin;
}

void linear_reset_globals()
{
    for (uint32_t i = 0; i < ARRAY_SIZE(global_linear_allocators); ++i)
    {
        if (global_linear_allocators[i])
        {
            linear_reset(global_linear_allocators[i]);
        }
    }
}
