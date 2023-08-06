#pragma once

#include <cstdint>
#include <cstdlib>

#include "bul/bul.h"

namespace bul
{
class RingBuffer
{
public:
    RingBuffer(size_t size, size_t alignment = 1)
        : size_(size)
        , alignment_(alignment)
        , allocated_(true)
    {
        data_ = (uint8_t*)malloc(size);
    }

    RingBuffer(void* data, size_t size, size_t alignment = 1)
        : data_((uint8_t*)data)
        , size_(size)
        , alignment_(alignment)
    {}

    ~RingBuffer()
    {
        if (allocated_)
        {
            free(data_);
            data_ = nullptr;
        }
    }

    void* push(const void* data, uint32_t size)
    {
        uint32_t aligned_size = size + alignment_ - size % alignment_;
        if (offset_ + size > size_)
        {
            offset_ = 0;
        }
        memcpy(bul::ptr_offset(data_, offset_), data, size);
        uint32_t ret = offset_;
        offset_ += aligned_size;
        return ret;
    }

    template <typename T>
    T& push(const T& data)
    {
        uint32_t aligned_size = sizeof(T) + alignment_ - sizeof(T) % alignment_;
        if (offset_ + sizeof(T) > size_)
        {
            offset_ = 0;
        }
        *bul::ptr_offset<T*>(data_, offset_) = data;
        uint32_t ret = offset_;
        offset_ += aligned_size;
        return ret;
    }

    template <typename T>
    T& push(T&& data)
    {
        uint32_t aligned_size = sizeof(T) + alignment_ - sizeof(T) % alignment_;
        if (offset_ + sizeof(T) > size_)
        {
            offset_ = 0;
        }
        *bul::ptr_offset<T*>(data_, offset_) = std::move(data);
        uint32_t ret = offset_;
        offset_ += aligned_size;
        return ret;
    }

private:
    uint8_t* data_ = nullptr;
    uint32_t size_ = 0;
    uint32_t alignment_ = 0;
    uint32_t offset_ = 0;
    bool allocated_ = false;
};
} // namespace bul
