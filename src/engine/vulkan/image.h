#pragma once

#include <string_view>
#include <volk.h>
#include <vma/vk_mem_alloc.h>

#include "bul/containers/handle.h"

namespace vk
{
enum class ImageUsage
{
    None,
    GraphicsShaderRead,
    GraphicsShaderReadWrite,
    ComputeShaderRead,
    ComputeShaderReadWrite,
    TransferDst,
    TransferSrc,
    ColorAttachment,
    DepthAttachment,
    Present
};

struct ImageAccess
{
    VkPipelineStageFlags stage = 0;
    VkAccessFlags access = 0;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

struct ImageView
{
    VkImageView vk_handle = VK_NULL_HANDLE;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkImageSubresourceRange range;
};

struct ImageDescription
{
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    VkImageType type = VK_IMAGE_TYPE_2D;
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;
    uint32_t mip_levels = 1;
    VkImageUsageFlags usage =
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_GPU_ONLY;
};

struct Image
{
    static bul::Handle<Image> create(const ImageDescription& description, VkImage vk_image = VK_NULL_HANDLE);
    static bul::Handle<Image> create(const ImageDescription& description, const std::string_view path);
    static void destroy(bul::Handle<Image> handle);
    void destroy();

    ImageDescription description;
    VkImage vk_handle = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    ImageView full_view;
    ImageUsage usage = ImageUsage::None;
};
} // namespace vk
