#include "image.h"

#include <stb/stb_image.h>

#include "vk_tools.h"
#include "device.h"

namespace vk
{
static ImageView create_image_view(VkImage image, const VkImageSubresourceRange& range, VkFormat format,
                                   VkImageViewType type)
{
    ImageView view{};
    view.range = range;
    view.format = format;

    VkImageViewCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    create_info.flags = 0;
    create_info.image = image;
    create_info.format = format;
    create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    create_info.subresourceRange = range;
    create_info.viewType = type;

    VK_CHECK(vkCreateImageView(device::vk_handle, &create_info, nullptr, &view.vk_handle));

    return view;
}

bul::handle<Image> Image::create(const ImageDescription& description, VkImage vk_image)
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

        VK_CHECK(vmaCreateImage(device::allocator, &image_info, &alloc_info, &vk_image, &allocation, nullptr));
    }

    VkImageSubresourceRange full_range{};
    full_range.aspectMask = is_depth(description.format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    full_range.baseMipLevel = 0;
    full_range.levelCount = description.mip_levels;
    full_range.baseArrayLayer = 0;
    full_range.layerCount = 1;

    ImageView full_view =
        create_image_view(vk_image, full_range, description.format, static_cast<VkImageViewType>(description.type));

    return device::images.insert(
        Image{.description = description, .vk_handle = vk_image, .allocation = allocation, .full_view = full_view});
}

bul::handle<Image> Image::create(const ImageDescription& description, const std::string_view path)
{
    int width, height, channels;
    if (!stbi_info(path.data(), &width, &height, &channels))
    {
        return bul::handle<Image>::invalid;
    }

    ImageDescription new_description = description;
    new_description.width = width;
    new_description.height = height;
    new_description.depth = 1;

    return create(new_description);
}

void Image::destroy(bul::handle<Image> handle)
{
    device::images.get(handle).destroy();
    device::images.erase(handle);
}

void Image::destroy()
{
    if (allocation != VK_NULL_HANDLE)
    {
        vmaDestroyImage(device::allocator, vk_handle, allocation);
        allocation = VK_NULL_HANDLE;
        vk_handle = VK_NULL_HANDLE;
    }
    vkDestroyImageView(device::vk_handle, full_view.vk_handle, nullptr);
    full_view.vk_handle = VK_NULL_HANDLE;
}
} // namespace vk
