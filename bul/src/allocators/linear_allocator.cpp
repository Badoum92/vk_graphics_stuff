#include "bul/allocators/linear_allocator.h"

namespace bul
{
constexpr uint32_t linear_allocator_size = 256_KB;
static thread_local uint8_t linear_allocator_buffer[linear_allocator_size];
thread_local linear_allocator g_linear_allocator =
    linear_allocator::create(linear_allocator_buffer, linear_allocator_size);

linear_allocator linear_allocator::create(void* buffer, uint32_t size)
{
    return {(uint8_t*)buffer, ptr_offset<uint8_t*>(buffer, size), (uint8_t*)buffer};
}

void* linear_allocator::alloc(uint32_t size)
{
    uint8_t* ptr = current;
    current += size;
    ASSERT(current <= end);
    return ptr;
}

void* linear_allocator::alloc_aligned(uint32_t size, uint32_t alignment)
{
    current = (uint8_t*)align(current, alignment);
    return alloc(size);
}

void linear_allocator::reset(void* ptr)
{
    ASSERT(begin <= ptr && ptr <= end);
    current = (uint8_t*)ptr;
}
} // namespace bul