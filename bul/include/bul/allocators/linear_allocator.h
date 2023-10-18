#pragma once

#include "bul/bul.h"

namespace bul
{
struct linear_allocator
{
    linear_allocator(void* buffer, uint32_t size)
        : _current(buffer)
        , _end(_current + size)
    {}

    void* alloc(uint32_t size)
    {
        void* ptr = _current;
        _current += size;
        ASSERT(_current <= _end);
        return ptr;
    }

    void* alloc_aligned(uint32_t size, uint32_t alignment = 16)
    {
        _current = align_pow2(_current, alignment);
        return alloc(size);
    }

    void rewind(void* ptr)
    {
        _current = ptr;
    }

    uint8_t* _current = nullptr;
    uint8_t* _end = nullptr;
};
} // namespace bul
