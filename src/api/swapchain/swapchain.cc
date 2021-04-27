#include "swapchain.hh"

#include <cstdint>
#include <algorithm>

#include "window/window.hh"
#include "vk_context/vk_context.hh"

static VkSurfaceFormatKHR choose_swap_surface_format(const SwapChainSupportDetails& details)
{
    for (const auto& format : details.formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }
    return details.formats[0];
}

static VkPresentModeKHR choose_swap_present_mode(const SwapChainSupportDetails& details)
{
    for (const auto& present_mode : details.present_modes)
    {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return present_mode;
        }
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D choose_swap_extent(const SwapChainSupportDetails& details)
{
    const auto& capabilities = details.capabilities;
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    }
    else
    {
        VkExtent2D extent = {static_cast<uint32_t>(Window::width()), static_cast<uint32_t>(Window::height())};
        const auto& min_extent = capabilities.minImageExtent;
        const auto& max_extent = capabilities.maxImageExtent;
        extent.width = std::clamp(extent.width, min_extent.width, max_extent.width);
        extent.height = std::clamp(extent.height, min_extent.height, max_extent.height);
        return extent;
    }
}

void SwapChain::create()
{
    auto details = VkContext::physical_device.get_swapchain_support_details();
    auto surface_format = choose_swap_surface_format(details);
    auto present_mode = choose_swap_present_mode(details);
    auto extent = choose_swap_extent(details);

    uint32_t image_count = details.capabilities.minImageCount + 1;
    if (details.capabilities.maxImageCount > 0 && image_count > details.capabilities.maxImageCount)
    {
        image_count = details.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = VkContext::surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = VkContext::physical_device.get_queue_family_indices();
    uint32_t queue_family_indices[] = {indices.graphics_family.value(), indices.present_family.value()};
    if (indices.graphics_family != indices.present_family)
    {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = 2;
        create_info.pQueueFamilyIndices = queue_family_indices;
    }
    else
    {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        create_info.queueFamilyIndexCount = 0;
        create_info.pQueueFamilyIndices = nullptr;
    }

    create_info.preTransform = details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(VkContext::device, &create_info, nullptr, &handle_));

    vkGetSwapchainImagesKHR(VkContext::device, handle_, &image_count, nullptr);
    images_.resize(image_count);
    vkGetSwapchainImagesKHR(VkContext::device, handle_, &image_count, images_.data());

    format_ = surface_format.format;
    extent_ = extent;

    image_views_.resize(images_.size());

    for (size_t i = 0; i < images_.size(); ++i)
    {
        VkImageViewCreateInfo img_view_create_info{};
        img_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        img_view_create_info.image = images_[i];
        img_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        img_view_create_info.format = format_;
        img_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        img_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        img_view_create_info.subresourceRange.baseMipLevel = 0;
        img_view_create_info.subresourceRange.levelCount = 1;
        img_view_create_info.subresourceRange.baseArrayLayer = 0;
        img_view_create_info.subresourceRange.layerCount = 1;

        VK_CHECK(vkCreateImageView(VkContext::device, &img_view_create_info, nullptr, &image_views_[i]));
    }
}

void SwapChain::destroy()
{
    vkDestroySwapchainKHR(VkContext::device, handle_, nullptr);

    for (auto image_view : image_views_)
    {
        vkDestroyImageView(VkContext::device, image_view, nullptr);
    }
}

const VkFormat& SwapChain::format() const
{
    return format_;
}

const VkExtent2D& SwapChain::extent() const
{
    return extent_;
}

const std::vector<VkImageView>& SwapChain::image_views() const
{
    return image_views_;
}

size_t SwapChain::size() const
{
    return images_.size();
}
