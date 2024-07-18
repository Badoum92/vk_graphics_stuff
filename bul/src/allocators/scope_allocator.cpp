#include "bul/allocators/scope_allocator.h"

namespace bul
{
scope_allocator scope_allocator::create(linear_allocator* linear_allocator)
{
    return {linear_allocator, linear_allocator->current};
}

scope_allocator scope_allocator::create_global()
{
    return create(&g_linear_allocator);
}

scope_allocator::~scope_allocator()
{
    allocator->reset(begin);
}

void* scope_allocator::alloc(uint32_t size)
{
    return allocator->alloc(size);
}

void* scope_allocator::alloc_aligned(uint32_t size, uint32_t alignment)
{
    return allocator->alloc_aligned(size, alignment);
}
} // namespace bul