#pragma once

#include <volk.h>
#include <vma/vk_mem_alloc.h>

#include "fwd.h"
#include "bul/containers/handle.h"

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
    static bul::Handle<Buffer> create(const BufferDescription& description);
    static void destroy(bul::Handle<Buffer> handle);
    void destroy();
    void* map();
    void unmap();

    BufferDescription description;
    VkBuffer vk_handle;
    VmaAllocation allocation = VK_NULL_HANDLE;
    void* mapped_data = nullptr;
};

struct RingBuffer
{
    static RingBuffer create(const BufferDescription& description);
    void destroy();
    void* map();
    void unmap();
    uint32_t push(const void* data, uint32_t size);

    bul::Handle<Buffer> buffer_handle;
    void* mapped_data = nullptr;
    uint32_t size = 0;
    uint32_t alignment = 0;
    uint32_t offset = 0;
};
} // namespace vk
