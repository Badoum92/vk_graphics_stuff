#include "bul/allocators/scope_allocator.h"

#include <cstdlib>
#include <utility>

#include "bul/bul.h"

namespace bul
{
scope_allocator::scope_allocator(size_t capacity)
{
    begin = malloc(capacity);
    end = begin + capacity;
    previous = current;
    current = begin;
    free_function = ::free;
}

scope_allocator::scope_allocator(void* buffer, size_t size, free_t free_function_)
{
    begin = buffer;
    end = begin + size;
    previous = current;
    current = begin;
    free_function = free_function_;
}

scope_allocator::~scope_allocator()
{
    if (previous == begin)
    {
        if (free_function)
        {
            free_function(begin);
        }
        begin = nullptr;
        end = nullptr;
        current = nullptr;
    }
    else
    {
        current = previous;
        void** saved_previous = previous;
        previous = *previous;
    }
}

scope_allocator::scope_allocator(const scope_allocator& other)
{
    *this = other;
}

scope_allocator& scope_allocator::operator=(const scope_allocator& other)
{
    ASSERT(end - current > sizeof(void*));
    void** save_previous = current;
    *save_previous = previous;
    previous = current;
    current += sizeof(void*);
}

scope_allocator::scope_allocator(scope_allocator&& other)
{
    *this = std::move(other);
}

scope_allocator& scope_allocator::operator=(scope_allocator&& other)
{
    if (free_function)
    {
        free_function(begin);
    }
    begin = other.begin;
    end = other.end;
    previous = other.previous;
    current = other.current;
    free_function = other.free_function;
    other.begin = nullptr;
    other.end = nullptr;
    other.previous = nullptr;
    other.current = nullptr;
    other.free_function = nullptr;
    return *this;
}

void* scope_allocator::allocate(size_t size, size_t alignment)
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
