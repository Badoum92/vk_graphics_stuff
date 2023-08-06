#pragma once

#include <string>
#include <volk.h>
#include <vma/vk_mem_alloc.h>

#include "bul/containers/handle.h"

namespace vk
{
inline constexpr VkBufferUsageFlags storage_buffer_usage =
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
inline constexpr VkBufferUsageFlags index_buffer_usage =
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
inline constexpr VkBufferUsageFlags uniform_buffer_usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
inline constexpr VkBufferUsageFlags transfer_buffer_usage =
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
inline constexpr VkBufferUsageFlags indirect_buffer_usage =
    VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

struct BufferDescription
{
    uint32_t size = 0;
    VkBufferUsageFlags usage = storage_buffer_usage;
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
    std::string name;
};

struct Buffer
{
    BufferDescription description;
    VkBuffer vk_handle;
    VmaAllocation allocation = VK_NULL_HANDLE;
    void* mapped_data = nullptr;
};
} // namespace vk
