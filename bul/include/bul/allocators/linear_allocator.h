#pragma once

#include "bul/types.h"

namespace bul
{
class LinearAllocator
{
public:
    LinearAllocator(size_t capacity);
    ~LinearAllocator();

    LinearAllocator(const LinearAllocator&) = delete;
    LinearAllocator& operator=(const LinearAllocator&) = delete;

    LinearAllocator(LinearAllocator&& other);
    LinearAllocator& operator=(LinearAllocator&& other);

    void* allocate(size_t size, size_t alignment = 8);
    void free(void*) {}

    void clear() { current_ = begin_; }

private:
    uint8_t* begin_ = nullptr;
    uint8_t* end_ = nullptr;
    uint8_t* current_ = nullptr;
};
}
