#include "image_view.hh"

#include "vk_context/vk_context.hh"

void ImageView::create(VkImage image, VkImageType image_type, VkFormat image_format, VkImageAspectFlags image_aspect)
{
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image;
    view_info.viewType = static_cast<VkImageViewType>(image_type);
    view_info.format = image_format;
    view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.subresourceRange.aspectMask = image_aspect;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(VkContext::device, &view_info, nullptr, &handle_));
}

void ImageView::destroy()
{
    vkDestroyImageView(VkContext::device, handle_, nullptr);
}
