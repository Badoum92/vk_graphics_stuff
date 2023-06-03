#include "image.h"

#include <stb/stb_image.h>

#include "vk_tools.h"
#include "device.h"

namespace vk
{
static ImageView create_image_view(const Device& device, VkImage image, const VkImageSubresourceRange& range,
                                   VkFormat format, VkImageViewType type)
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

    VK_CHECK(vkCreateImageView(device.vk_handle, &create_info, nullptr, &view.vk_handle));

    return view;
}

bul::Handle<Image> Device::create_image(const ImageDescription& description, VkImage vk_image)
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

    ImageView full_view = create_image_view(*this, vk_image, full_range, description.format,
                                            static_cast<VkImageViewType>(description.type));

    return images.insert(
        Image{.description = description, .vk_handle = vk_image, .allocation = allocation, .full_view = full_view});
}

bul::Handle<Image> Device::create_image(const ImageDescription& description, const std::string& path)
{
    int width, height, channels;
    if (!stbi_info(path.c_str(), &width, &height, &channels))
    {
        return bul::Handle<Image>::invalid;
    }

    ImageDescription new_description = description;
    new_description.width = width;
    new_description.height = height;
    new_description.depth = 1;

    return create_image(new_description);
}

void Device::destroy_image(Image& image)
{
    if (image.allocation != VK_NULL_HANDLE)
    {
        vmaDestroyImage(allocator, image.vk_handle, image.allocation);
        image.allocation = VK_NULL_HANDLE;
        image.vk_handle = VK_NULL_HANDLE;
    }
    vkDestroyImageView(vk_handle, image.full_view.vk_handle, nullptr);
    image.full_view.vk_handle = VK_NULL_HANDLE;
}
} // namespace vk
