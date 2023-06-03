#pragma once

#include <volk.h>
#include <vma/vk_mem_alloc.h>

#include "fwd.h"
#include "handle.hh"

namespace vk
{
struct BufferDescription
{
    uint32_t size = 0;
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_GPU_ONLY;
};

struct Buffer
{
    BufferDescription description;
    VkBuffer vk_handle;
    VmaAllocation allocation = VK_NULL_HANDLE;
    void* mapped_data = nullptr;
};

struct RingBuffer
{
    static RingBuffer create(Device& device, const BufferDescription& description);
    uint32_t push(const void* data, uint32_t size);

    Handle<Buffer> buffer_handle;
    BufferDescription description;
    uint8_t* mapped_data = nullptr;
    uint32_t alignment = 0;
    uint32_t offset = 0;
};
} // namespace vk
