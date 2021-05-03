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

    VK_CHECK(vmaCreateBuffer(VkContext::allocator, &buffer_info, &alloc_info, &handle_, &alloc_, nullptr));
}

void Buffer::destroy()
{
    if (size_ != 0)
    {
        if (mapped_data_ != nullptr)
        {
            vmaUnmapMemory(VkContext::allocator, alloc_);
            mapped_data_ = nullptr;
        }
        vmaDestroyBuffer(VkContext::allocator, handle_, alloc_);
        size_ = 0;
    }
}

void Buffer::fill(const Buffer& staging_buffer)
{
    assert(size_ <= staging_buffer.size_);

    vk_execute_once([&](VkCommandBuffer cmd) {
        VkBufferCopy copy_region{};
        copy_region.srcOffset = 0;
        copy_region.dstOffset = 0;
        copy_region.size = size_;
        vkCmdCopyBuffer(cmd, staging_buffer, handle_, 1, &copy_region);
    });
}

void Buffer::write_at(const void* data, size_t size, size_t offset)
{
    bool map_unmap = mapped_data_ == nullptr;
    if (map_unmap)
        vmaMapMemory(VkContext::allocator, alloc_, &mapped_data_);
    memcpy(reinterpret_cast<uint8_t*>(mapped_data_) + offset, data, size);
    if (map_unmap)
    {
        vmaUnmapMemory(VkContext::allocator, alloc_);
        mapped_data_ = nullptr;
    }
}

void Buffer::create_staging(const void* data, size_t size)
{
    create(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
    write_at(data, size, 0);
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

void Buffer::create_storage(const void* data, size_t size)
{
    Buffer staging;
    staging.create_staging(data, size);
    create(size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
    fill(staging);
}

void Buffer::create_uniform(size_t size)
{
    create(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
    vmaMapMemory(VkContext::allocator, alloc_, &mapped_data_);
}

VkBufferUsageFlags Buffer::buffer_usage() const
{
    return buffer_usage_;
}

size_t Buffer::size() const
{
    return size_;
}

size_t Buffer::push(const void* data, size_t size)
{
    if (offset_ + size >= size_)
    {
        offset_ = 0;
    }

    bool map_unmap = mapped_data_ == nullptr;
    if (map_unmap)
        vmaMapMemory(VkContext::allocator, alloc_, &mapped_data_);
    memcpy(reinterpret_cast<uint8_t*>(mapped_data_) + offset_, data, size);
    if (map_unmap)
    {
        vmaUnmapMemory(VkContext::allocator, alloc_);
        mapped_data_ = nullptr;
    }

    size_t ret = offset_;
    offset_ += size;
    return ret;
}
