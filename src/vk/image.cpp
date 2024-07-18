#include "vk/image.h"

#include "vk/context.h"

namespace vk
{
static image_view create_image_view(context* context, const image_description& description, VkImage vk_image)
{
    image_view view = {};
    view.range.aspectMask = is_depth(description.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    view.range.baseMipLevel = 0;
    view.range.levelCount = description.mip_levels;
    view.range.baseArrayLayer = 0;
    view.range.layerCount = 1;
    view.format = description.format;

    VkImageViewCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.flags = 0;
    create_info.image = vk_image;
    create_info.format = description.format;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange = view.range;
    create_info.viewType = (VkImageViewType)description.type;

    VK_CHECK(vkCreateImageView(context->device, &create_info, nullptr, &view.vk_handle));
    return view;
}

bul::handle<image> context::create_image(const image_description& description, VkImage vk_image)
{
    VmaAllocation allocation = VK_NULL_HANDLE;
    if (vk_image == VK_NULL_HANDLE)
    {
        VkImageCreateInfo image_info{};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = description.type;
        image_info.extent.width = description.width;
        image_info.extent.height = description.height;
        image_info.extent.depth = description.depth;
        image_info.mipLevels = description.mip_levels;
        image_info.arrayLayers = 1;
        image_info.format = description.format;
        image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.usage = description.usage;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = description.sample_count;
        image_info.flags = 0;

        VmaAllocationCreateInfo alloc_info{};
        alloc_info.usage = description.memory_usage;

        VK_CHECK(vmaCreateImage(allocator, &image_info, &alloc_info, &vk_image, &allocation, nullptr));
    }

    VkImageSubresourceRange full_range{};
    full_range.aspectMask = is_depth(description.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    full_range.baseMipLevel = 0;
    full_range.levelCount = description.mip_levels;
    full_range.baseArrayLayer = 0;
    full_range.layerCount = 1;

    image_view full_view = create_image_view(this, description, vk_image);

    if (description.name)
    {
        set_resource_name(this, (uint64_t)vk_image, VK_OBJECT_TYPE_IMAGE, description.name);
        set_resource_name(this, (uint64_t)full_view.vk_handle, VK_OBJECT_TYPE_IMAGE_VIEW, description.name);
    }

    return images.insert(image{vk_image, allocation, full_view, image_usage::none, description});
}

void context::destroy_image(bul::handle<image> handle)
{
    image& image = images.get(handle);
    if (image.full_view.vk_handle != VK_NULL_HANDLE)
    {
        vkDestroyImageView(device, image.full_view.vk_handle, nullptr);
        image.full_view.vk_handle = VK_NULL_HANDLE;
    }
    if (image.allocation != VK_NULL_HANDLE)
    {
        vmaDestroyImage(allocator, image.vk_handle, image.allocation);
        image.allocation = VK_NULL_HANDLE;
        image.vk_handle = VK_NULL_HANDLE;
    }
    images.erase(handle);
}

bul::handle<sampler> context::create_sampler(const sampler_description& description)
{
    VkSamplerCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.pNext = nullptr;
    create_info.flags = 0;
    create_info.magFilter = description.filter;
    create_info.minFilter = description.filter;
    create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    create_info.addressModeU = description.address_mode;
    create_info.addressModeV = description.address_mode;
    create_info.addressModeW = description.address_mode;
    create_info.mipLodBias = 0;
    create_info.anisotropyEnable = false;
    create_info.maxAnisotropy = 0.0f;
    create_info.compareEnable = false;
    create_info.compareOp = VK_COMPARE_OP_NEVER;
    create_info.minLod = description.min_lod;
    create_info.maxLod = description.max_lod;
    create_info.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    create_info.unnormalizedCoordinates = false;

    VkSampler vk_sampler;
    VK_CHECK(vkCreateSampler(device, &create_info, nullptr, &vk_sampler));

    return samplers.insert(sampler{vk_sampler, description});
}

void context::destroy_sampler(bul::handle<sampler> handle)
{
    sampler& sampler = samplers.get(handle);
    vkDestroySampler(device, sampler.vk_handle, nullptr);
}
} // namespace vk
