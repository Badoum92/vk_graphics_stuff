#include "vk/buffer.h"

#include "vk/vk_tools.h"
#include "vk/context.h"

#include "bul/bul.h"

namespace vk
{
bul::handle<buffer> context::create_buffer(const buffer_description& description)
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = description.size;
    buffer_info.usage = description.usage | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    bool host_accessible = description.usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
        || description.usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        || description.usage & VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = description.memory_usage;
    if (host_accessible)
    {
        alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
    }

    VkBuffer vk_buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VK_CHECK(vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &vk_buffer, &allocation, nullptr));

    VkBufferDeviceAddressInfo buffer_device_address_info = {};
    buffer_device_address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    buffer_device_address_info.buffer = vk_buffer;
    VkDeviceAddress device_address = vkGetBufferDeviceAddress(device, &buffer_device_address_info);

    void* mapped_data = nullptr;
    if (host_accessible)
    {
        vmaMapMemory(allocator, allocation, &mapped_data);
    }

    if (description.name)
    {
        set_resource_name(this, (uint64_t)vk_buffer, VK_OBJECT_TYPE_BUFFER, description.name);
    }

    return buffers.insert(buffer{vk_buffer, allocation, device_address, mapped_data, description});
}

void context::destroy_buffer(bul::handle<buffer> handle)
{
    buffer& buffer = buffers.get(handle);
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
