#include "bul/allocators/linear_allocator.h"

#include <bit>
#include <cstdlib>

#include "bul/bul.h"

namespace bul
{
LinearAllocator::LinearAllocator(size_t capacity)
{
    begin_ = malloc(capacity);
    end_ = begin_ + capacity;
    current_ = begin_;
}

LinearAllocator::~LinearAllocator()
{
    ::free(begin_);
    begin_ = nullptr;
    end_ = nullptr;
    current_ = nullptr;
}

LinearAllocator::LinearAllocator(LinearAllocator&& other)
{
    *this = std::move(other);
}

LinearAllocator::LinearAllocator& operator=(LinearAllocator&& other)
{
    ::free(begin_);
    begin_ = other.begin_;
    current_ = other.current_;
    capacity_ = other.capacity_;
    other.begin_ = nullptr;
    other.current_ = nullptr;
    other.capacity_ = 0;
}

void* LinearAllocator::allocate(size_t size, size_t alignment)
{
    ASSERT(std::has_single_bit(alignment));
    current_ = align_pow2(current_, alignment);
    ASSERT(current_ + size < end_);
    uint8_t* ret = current_;
    current_ += size;
    return ret;
}
}
