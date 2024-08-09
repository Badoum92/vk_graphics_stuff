#pragma once

#include <volk.h>
#include <vma/vk_mem_alloc.h>

namespace vk
{
constexpr VkImageUsageFlags image_usage_depth_attachment = VK_IMAGE_USAGE_TRANSFER_SRC_BIT
    | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
constexpr VkImageUsageFlags image_usage_sampled =
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
constexpr VkImageUsageFlags image_usage_storage =
    VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
constexpr VkImageUsageFlags image_usage_color_attachment =
    image_usage_storage | image_usage_sampled | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

enum class image_usage
{
    none,
    graphics_shader_read,
    graphics_shader_read_write,
    compute_shader_read,
    compute_shader_read_write,
    transfer_dst,
    transfer_src,
    color_attachment,
    depth_attachment,
    present
};

struct image_access
{
    VkPipelineStageFlags stage = 0;
    VkAccessFlags access = 0;
    VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
};

struct image_view
{
    VkImageView vk_handle = VK_NULL_HANDLE;
    VkFormat format = VK_FORMAT_UNDEFINED;
    VkImageSubresourceRange range;
};

struct image_description
{
    uint32_t width = 1;
    uint32_t height = 1;
    uint32_t depth = 1;
    uint32_t mip_levels = 1;
    VkImageType type = VK_IMAGE_TYPE_2D;
    VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;
    VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT;
    VkImageUsageFlags usage = image_usage_sampled;
    VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_AUTO;
    const char* name;
};

struct image
{
    VkImage vk_handle = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    image_view full_view;
    image_usage usage = image_usage::none;
    image_description description;
};

struct sampler_description
{
    VkFilter filter = VK_FILTER_NEAREST;
    VkSamplerAddressMode address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    float min_lod = 0.0f;
    float max_lod = VK_LOD_CLAMP_NONE;
};

struct sampler
{
    VkSampler vk_handle = VK_NULL_HANDLE;
    sampler_description description;
};

struct load_op
{
    static load_op load()
    {
        load_op op;
        op.vk_loadop = VK_ATTACHMENT_LOAD_OP_LOAD;
        return op;
    }

    static load_op dont_care()
    {
        load_op op;
        op.vk_loadop = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        return op;
    }

    static load_op clear_color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f)
    {
        load_op op;
        op.vk_loadop = VK_ATTACHMENT_LOAD_OP_CLEAR;
        op.clear_value.color.float32[0] = r;
        op.clear_value.color.float32[1] = g;
        op.clear_value.color.float32[2] = b;
        op.clear_value.color.float32[3] = a;
        return op;
    }

    static load_op clear_depth(float depth = 0.0f, uint32_t stencil = 0)
    {
        load_op op;
        op.vk_loadop = VK_ATTACHMENT_LOAD_OP_CLEAR;
        op.clear_value.depthStencil = {depth, stencil};
        return op;
    }

    VkAttachmentLoadOp vk_loadop = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkClearValue clear_value;
};
} // namespace vk
