#include "vk/buffer.h"

#include "vk/vk_tools.h"
#include "vk/context.h"

#include "bul/bul.h"

namespace vk
{
bul::Handle<Buffer> Context::create_buffer(BufferDescription&& description)
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = description.size;
    buffer_info.usage = description.usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = description.memory_usage;
    if (description.memory_usage == VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE)
    {
        alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    }

    VkBuffer vk_buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VK_CHECK(vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &vk_buffer, &allocation, nullptr));

    set_resource_name(this, (uint64_t)vk_buffer, VK_DEBUG_REPORT_OBJECT_TYPE_BUFFER_EXT, description.name);

    void* mapped_data = nullptr;
    vmaMapMemory(allocator, allocation, &mapped_data);

    return buffers.insert({.description = std::move(description),
                           .vk_handle = vk_buffer,
                           .allocation = allocation,
                           .mapped_data = mapped_data});
}

void Context::destroy_buffer(bul::Handle<Buffer> handle)
{
    if (!handle)
    {
        return;
    }

    Buffer& buffer = buffers.get(handle);
    if (buffer.mapped_data != nullptr)
    {
        vmaUnmapMemory(allocator, buffer.allocation);
        buffer.mapped_data = nullptr;
    }
    if (buffer.allocation != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(allocator, buffer.vk_handle, buffer.allocation);
        buffer.allocation = VK_NULL_HANDLE;
        buffer.vk_handle = VK_NULL_HANDLE;
    }
    buffers.erase(handle);
}
} // namespace vk
