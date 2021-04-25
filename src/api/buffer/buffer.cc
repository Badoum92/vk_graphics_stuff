#include "buffer.hh"

#include <cassert>

#include "vk_context/vk_context.hh"

Buffer::Buffer(size_t size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage)
{
    create(size, buffer_usage, memory_usage);
}

Buffer::~Buffer()
{
    destroy();
}

void Buffer::create(size_t size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage)
{
    destroy();

    size_ = size;
    buffer_usage_ = buffer_usage;
    memory_usage_ = memory_usage;

    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = buffer_usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = memory_usage;

    vmaCreateBuffer(VkContext::allocator, &buffer_info, &alloc_info, &handle_, &alloc_, nullptr);
}

void Buffer::destroy()
{
    if (size_ != 0)
    {
        vmaDestroyBuffer(VkContext::allocator, handle_, alloc_);
        size_ = 0;
    }
}

void Buffer::fill(const void* data, size_t size)
{
    void* mapped_data;
    vmaMapMemory(VkContext::allocator, alloc_, &mapped_data);
    memcpy(mapped_data, data, size);
    vmaUnmapMemory(VkContext::allocator, alloc_);
}

void Buffer::fill(const Buffer& staging_buffer)
{
    assert(size_ == staging_buffer.size_);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = VkContext::command_pool;
    alloc_info.commandBufferCount = 1;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(VkContext::device, &alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(command_buffer, &begin_info);

    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = 0;
    copy_region.size = size_;
    vkCmdCopyBuffer(command_buffer, staging_buffer.handle_, handle_, 1, &copy_region);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    vkQueueSubmit(VkContext::device.graphics_queue(), 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(VkContext::device.graphics_queue());

    vkFreeCommandBuffers(VkContext::device, VkContext::command_pool, 1, &command_buffer);
}

void Buffer::create_staging(const void* data, size_t size)
{
    create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
    fill(data, size);
}

void Buffer::create_vertex(const void* data, size_t size)
{
    Buffer staging;
    staging.create_staging(data, size);
    create(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    fill(staging);
}

void Buffer::create_index(const void* data, size_t size)
{
    Buffer staging;
    staging.create_staging(data, size);
    create(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    fill(staging);
}

void Buffer::bind_vertex(const VkCommandBuffer& cmd, uint32_t binding, size_t offset) const
{
    assert(buffer_usage_ & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    vkCmdBindVertexBuffers(cmd, binding, 1, &handle_, &offset);
}

void Buffer::bind_index(const VkCommandBuffer& cmd, size_t offset, VkIndexType type) const
{
    assert(buffer_usage_ & VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
    vkCmdBindIndexBuffer(cmd, handle_, offset, type);
}
