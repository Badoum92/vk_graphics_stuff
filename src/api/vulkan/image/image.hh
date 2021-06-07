#pragma once

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>
#include <filesystem>

#include "image_view/image_view.hh"

class Image
{
public:
    ~Image();

    void create(uint32_t width, uint32_t height, uint32_t depth, VkFormat format, VkImageUsageFlags usage,
                VkSampleCountFlagBits sample_count);
    void destroy();

    void create(const std::filesystem::path& path);
    void create(uint32_t width, uint32_t height, uint32_t depth = 1);
    void create_depth_attachment(uint32_t width, uint32_t height);

    void fill(const void* data, size_t size);

    void transition_layout(VkImageLayout new_layout);

    const ImageView& view() const;
    VkFormat format() const;
    VkImageType type() const;
    VkImageLayout layout() const;

    inline const VkImage& handle() const
    {
        return handle_;
    }

    inline operator const VkImage&() const
    {
        return handle_;
    }

private:
    VkImage handle_ = VK_NULL_HANDLE;
    VmaAllocation alloc_;
    ImageView view_;

    VkFormat format_;
    VkImageUsageFlags usage_;
    VkSampleCountFlagBits sample_count_;
    VkImageType type_;
    VkImageLayout layout_;

    uint32_t width_;
    uint32_t height_;
    uint32_t depth_;
};
