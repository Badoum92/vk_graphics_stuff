#include "bul/allocators/linear_allocator.h"

#include <cstdlib>
#include <utility>

#include "bul/bul.h"

namespace bul
{
linear_allocator::linear_allocator(size_t capacity)
{
    begin = malloc(capacity);
    end = begin + capacity;
    current = begin;
    free_function = ::free;
}

linear_allocator::linear_allocator(void* buffer, size_t size, free_t free_function_)
{
    begin = buffer;
    end = begin + size;
    current = begin;
    free_function = free_function_;
}

linear_allocator::~linear_allocator()
{
    if (free_function)
    {
        free_function(begin);
    }
    begin = nullptr;
    end = nullptr;
    current = nullptr;
}

linear_allocator::linear_allocator(linear_allocator&& other)
{
    *this = std::move(other);
}

linear_allocator& linear_allocator::operator=(linear_allocator&& other)
{
    if (free_function)
    {
        free_function(begin);
    }
    begin = other.begin;
    end = other.end;
    current = other.current;
    free_function = other.free_function;
    other.begin = nullptr;
    other.end = nullptr;
    other.current = nullptr;
    other.free_function = nullptr;
    return *this;
}

void* linear_allocator::allocate(size_t size, size_t alignment)
{
    current = align_pow2(current, alignment);
    ASSERT(current + size < end);
    if (current + size >= end)
    {
        return nullptr;
    }
    uint8_t* ret = current;
    current += size;
    return ret;
}
} // namespace bul
