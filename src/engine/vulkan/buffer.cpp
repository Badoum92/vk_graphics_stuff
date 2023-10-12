#include "buffer.h"

#include "vk_tools.h"
#include "context.h"
#include "device.h"

#include "bul/bul.h"

namespace vk
{
bul::handle<Buffer> Buffer::create(const BufferDescription& description)
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = description.size;
    buffer_info.usage = description.usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = description.memory_usage;

    VkBuffer vk_buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VK_CHECK(vmaCreateBuffer(device::allocator, &buffer_info, &alloc_info, &vk_buffer, &allocation, nullptr));

    return device::buffers.insert({.description = description, .vk_handle = vk_buffer, .allocation = allocation});
}

void Buffer::destroy(bul::handle<Buffer> handle)
{
    device::buffers.get(handle).destroy();
    device::buffers.erase(handle);
}

void Buffer::destroy()
{
    unmap();
    if (allocation != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(device::allocator, vk_handle, allocation);
        allocation = VK_NULL_HANDLE;
        vk_handle = VK_NULL_HANDLE;
    }
}

void* Buffer::map()
{
    if (mapped_data == nullptr)
    {
        vmaMapMemory(device::allocator, allocation, &mapped_data);
    }
    return mapped_data;
}

void Buffer::unmap()
{
    if (mapped_data != nullptr)
    {
        vmaUnmapMemory(device::allocator, allocation);
        mapped_data = nullptr;
    }
}

RingBuffer RingBuffer::create(const BufferDescription& description)
{
    RingBuffer ring_buffer{};
    ring_buffer.alignment = context::physical_device.properties.limits.minUniformBufferOffsetAlignment;
    ring_buffer.size = description.size;
    ring_buffer.buffer_handle = Buffer::create(description);
    ring_buffer.mapped_data = device::buffers.get(ring_buffer.buffer_handle).map();
    return ring_buffer;
}

void RingBuffer::destroy()
{
    Buffer::destroy(buffer_handle);
}

void* RingBuffer::map()
{
    mapped_data = device::buffers.get(buffer_handle).map();
    return mapped_data;
}

void RingBuffer::unmap()
{
    device::buffers.get(buffer_handle).unmap();
}

uint32_t RingBuffer::push(const void* data, uint32_t size)
{
    uint32_t aligned_size = size + alignment - size % alignment;

    if (offset + size > this->size)
    {
        offset = 0;
    }

    memcpy(bul::ptr_offset(mapped_data, offset), data, size);

    uint32_t ret = offset;
    offset += aligned_size;
    return ret;
}
} // namespace vk
