#include "vk/image.h"

#include "vk/context.h"

namespace vk
{
static ImageView create_image_view(Context* context, const ImageDescription& description, VkImage vk_image)
{
    ImageView view{};
    view.range.aspectMask = is_depth(description.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    view.range.baseMipLevel = 0;
    view.range.levelCount = description.mip_levels;
    view.range.baseArrayLayer = 0;
    view.range.layerCount = 1;
    view.format = description.format;

    VkImageViewCreateInfo create_info{};
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

bul::Handle<Image> Context::create_image(ImageDescription&& description, VkImage vk_image)
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

    ImageView full_view = create_image_view(this, description, vk_image);

    if (!description.name.empty())
    {
        set_resource_name(this, (uint64_t)vk_image, VK_DEBUG_REPORT_OBJECT_TYPE_IMAGE_EXT, description.name);
    }

    return images.insert(Image{.description = std::move(description),
                               .vk_handle = vk_image,
                               .allocation = allocation,
                               .full_view = full_view});
}

void Context::destroy_image(bul::Handle<Image> handle)
{
    if (!handle)
    {
        return;
    }

    Image& image = images.get(handle);
    if (image.allocation != VK_NULL_HANDLE)
    {
        vmaDestroyImage(allocator, image.vk_handle, image.allocation);
        image.allocation = VK_NULL_HANDLE;
        image.vk_handle = VK_NULL_HANDLE;
    }
    vkDestroyImageView(device, image.full_view.vk_handle, nullptr);
    image.full_view.vk_handle = VK_NULL_HANDLE;
    images.erase(handle);
}
} // namespace vk
