#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "handle.hh"

namespace vk
{
struct Device;

struct BufferDescription
{
    uint32_t size = 0;
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
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
    template <typename T>
    static RingBuffer create(Device& device, const BufferDescription& description)
    {
        uint32_t n = description.size / sizeof(T);
        uint32_t alignment = device.physical_device.properties.limits.minUniformBufferOffsetAlignment;

        RingBuffer ring_buffer{};
        ring_buffer.elt_size = sizeof(T) + alignment - sizeof(T) % alignment;
        ring_buffer.description.usage = description.usage;
        ring_buffer.description.memory_usage = description.memory_usage;
        ring_buffer.description.size = ring_buffer.elt_size * n;
        ring_buffer.buffer_handle = device.create_buffer(ring_buffer.description);
        ring_buffer.mapped_data = device.map_buffer(ring_buffer.buffer_handle);

        return ring_buffer;
    }

    template <typename T>
    uint32_t push(const T& elt)
    {
        if (offset + elt_size > description.size)
        {
            offset = 0;
        }

        std::memcpy(reinterpret_cast<uint8_t*>(mapped_data) + offset, &elt, elt_size);

        uint32_t ret = offset;
        offset += elt_size;
        return ret;
    }

    Handle<Buffer> buffer_handle;
    BufferDescription description;
    void* mapped_data = nullptr;
    uint32_t offset = 0;
    uint32_t elt_size = 0;
};
} // namespace vk
