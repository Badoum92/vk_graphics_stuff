#include "buffer.hh"

#include "vk_tools.hh"
#include "device.hh"

namespace vk
{
Handle<Buffer> Device::create_buffer(const BufferDescription& description)
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
    VK_CHECK(vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &vk_buffer, &allocation, nullptr));

    return buffers.insert({.description = description, .vk_handle = vk_buffer, .allocation = allocation});
}

void Device::destroy_buffer(const Handle<Buffer>& handle)
{
    if (!handle.is_valid())
    {
        return;
    }

    Buffer& buffer = buffers.get(handle);
    unmap_buffer(handle);
    if (buffer.allocation != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(allocator, buffer.vk_handle, buffer.allocation);
        buffer.allocation = VK_NULL_HANDLE;
        buffer.vk_handle = VK_NULL_HANDLE;
    }
    buffers.remove(handle);
}

void* Device::map_buffer(const Handle<Buffer>& handle)
{
    Buffer& buffer = buffers.get(handle);
    if (buffer.mapped_data == nullptr)
    {
        vmaMapMemory(allocator, buffer.allocation, &buffer.mapped_data);
    }
    return buffer.mapped_data;
}

void Device::unmap_buffer(const Handle<Buffer>& handle)
{
    Buffer& buffer = buffers.get(handle);
    if (buffer.mapped_data != nullptr)
    {
        vmaUnmapMemory(allocator, buffer.allocation);
        buffer.mapped_data = nullptr;
    }
}
} // namespace vk
