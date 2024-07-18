#pragma once

#include <volk.h>
#include <vma/vk_mem_alloc.h>

#include "bul/containers/handle.h"

namespace vk
{
constexpr VkBufferUsageFlags storage_buffer_usage =
    VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
constexpr VkBufferUsageFlags index_buffer_usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
constexpr VkBufferUsageFlags vertex_buffer_usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
constexpr VkBufferUsageFlags uniform_buffer_usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
constexpr VkBufferUsageFlags transfer_buffer_usage =
    VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
constexpr VkBufferUsageFlags descriptor_buffer_usage =
    VK_BUFFER_USAGE_RESOURCE_DESCRIPTOR_BUFFER_BIT_EXT | VK_BUFFER_USAGE_SAMPLER_DESCRIPTOR_BUFFER_BIT_EXT;

struct buffer_description
{
    VkDeviceSize size = 0;
    VkBufferUsageFlags usage = storage_buffer_usage;
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO;
    const char* name = nullptr;
};

struct buffer
{
    VkBuffer vk_handle;
    VmaAllocation allocation = VK_NULL_HANDLE;
    VkDeviceAddress device_address;
    void* mapped_data = nullptr;
    buffer_description description;
};
} // namespace vk
