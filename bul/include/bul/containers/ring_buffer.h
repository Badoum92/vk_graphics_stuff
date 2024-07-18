#pragma once

#include <stdint.h>
#include <stdlib.h>

#include "bul/bul.h"

namespace bul
{
struct ring_buffer
{
    ring_buffer(void* data, uint32_t size)
        : _data((uint8_t*)data)
        , _size(size)
    {}

    void* push(const void* data, uint32_t size, uint32_t alignment)
    {
        uint32_t aligned_size = size + alignment - size % alignment;
        if (_offset + size > _size)
        {
            _offset = 0;
        }
        memcpy(bul::ptr_offset(_data, _offset), data, size);
        uint32_t ret = _offset;
        _offset += aligned_size;
        return ret;
    }

    template <typename T>
    T& push(const T& data, uint32_t alignment)
    {
        uint32_t aligned_size = sizeof(T) + alignment - sizeof(T) % alignment;
        if (_offset + sizeof(T) > _size)
        {
            _offset = 0;
        }
        *bul::ptr_offset<T*>(_data, _offset) = data;
        uint32_t ret = _offset;
        _offset += aligned_size;
        return ret;
    }

    template <typename T>
    T& push(T&& data, uint32_t alignment)
    {
        uint32_t aligned_size = sizeof(T) + alignment - sizeof(T) % alignment;
        if (_offset + sizeof(T) > _size)
        {
            _offset = 0;
        }
        *bul::ptr_offset<T*>(_data, _offset) = std::move(data);
        uint32_t ret = _offset;
        _offset += aligned_size;
        return ret;
    }

    uint8_t* _data = nullptr;
    uint32_t _size = 0;
    uint32_t _offset = 0;
};
} // namespace bul
