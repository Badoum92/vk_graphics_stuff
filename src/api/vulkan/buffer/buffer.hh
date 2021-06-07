#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

class Buffer
{
public:
    Buffer() = default;
    Buffer(size_t size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage);
    ~Buffer();

    void create(size_t size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage);
    void destroy();

    void fill(const Buffer& staging_buffer);

    void create_staging(const void* data, size_t size);
    void create_vertex(const void* data, size_t size);
    void create_index(const void* data, size_t size);
    void create_storage(const void* data, size_t size);
    void create_uniform(size_t size);

    VkBufferUsageFlags buffer_usage() const;

    size_t size() const;

    void write_at(const void* data, size_t size, size_t offset);
    template <typename T>
    inline void write_at(const T& element, size_t offset)
    {
        write_at(&element, sizeof(T), offset);
    }

    size_t push(const void* data, size_t size);
    template <typename T>
    inline size_t push(const T& element)
    {
        return push((&element), sizeof(T));
    }

    template <typename T>
    inline uint32_t count() const
    {
        return size_ / sizeof(T);
    }

    inline const VkBuffer& handle() const
    {
        return handle_;
    }

    inline operator const VkBuffer&() const
    {
        return handle_;
    }

private:
    VkBuffer handle_ = VK_NULL_HANDLE;
    VmaAllocation alloc_;
    void* mapped_data_{nullptr};

    size_t size_{0};
    size_t offset_{0};
    VkBufferUsageFlags buffer_usage_;
    VmaMemoryUsage memory_usage_;
};
