#include "swapchain.hh"

#include <cstdint>
#include <algorithm>

#include "window/window.hh"
#include "vk_context/vk_context.hh"

VkPresentModeKHR SwapChain::present_mode_;
VkSurfaceFormatKHR SwapChain::surface_format_;

void SwapChain::choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& formats)
{
    for (const auto& format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            surface_format_ = format;
            return;
        }
    }
    surface_format_ = formats[0];
}

void SwapChain::choose_swap_present_mode(const std::vector<VkPresentModeKHR>& present_modes)
{
    for (const auto& present_mode : present_modes)
    {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            present_mode_ = present_mode;
            return;
        }
    }
    present_mode_ = VK_PRESENT_MODE_FIFO_KHR;
}

void SwapChain::choose_swap_extent()
{
    if (surface_capabilities_.currentExtent.width != UINT32_MAX)
    {
        extent_ = surface_capabilities_.currentExtent;
    }
    else
    {
        extent_ = {static_cast<uint32_t>(Window::width()), static_cast<uint32_t>(Window::height())};
        const auto& min_extent = surface_capabilities_.minImageExtent;
        const auto& max_extent = surface_capabilities_.maxImageExtent;
        extent_.width = std::clamp(extent_.width, min_extent.width, max_extent.width);
        extent_.height = std::clamp(extent_.height, min_extent.height, max_extent.height);
    }
}

void SwapChain::create()
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkContext::physical_device, VkContext::surface, &surface_capabilities_);

    if (surface_capabilities_.currentExtent.width != UINT32_MAX)
    {
        extent_ = surface_capabilities_.currentExtent;
    }
    else
    {
        extent_ = {static_cast<uint32_t>(Window::width()), static_cast<uint32_t>(Window::height())};
        const auto& min_extent = surface_capabilities_.minImageExtent;
        const auto& max_extent = surface_capabilities_.maxImageExtent;
        extent_.width = std::clamp(extent_.width, min_extent.width, max_extent.width);
        extent_.height = std::clamp(extent_.height, min_extent.height, max_extent.height);
    }

    default_viewport_.x = 0.0f;
    default_viewport_.y = 0.0f;
    default_viewport_.width = static_cast<float>(extent_.width);
    default_viewport_.height = static_cast<float>(extent_.height);
    default_viewport_.minDepth = 0.0f;
    default_viewport_.maxDepth = 1.0f;

    default_scissor_.offset = {0, 0};
    default_scissor_.extent = extent_;

    uint32_t image_count = surface_capabilities_.minImageCount + 1;
    if (surface_capabilities_.maxImageCount > 0 && image_count > surface_capabilities_.maxImageCount)
    {
        image_count = surface_capabilities_.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = VkContext::surface;
    create_info.minImageCount = image_count;
    create_info.imageFormat = surface_format_.format;
    create_info.imageColorSpace = surface_format_.colorSpace;
    create_info.imageExtent = extent_;
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

    create_info.preTransform = surface_capabilities_.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode_;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(VkContext::device, &create_info, nullptr, &handle_));

    std::vector<VkImage> images;
    vkGetSwapchainImagesKHR(VkContext::device, handle_, &image_count, nullptr);
    images.resize(image_count);
    vkGetSwapchainImagesKHR(VkContext::device, handle_, &image_count, images.data());

    image_views_.resize(images.size());

    for (size_t i = 0; i < images.size(); ++i)
    {
        image_views_[i].create(images[i], VK_IMAGE_TYPE_2D, surface_format_.format, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void SwapChain::destroy()
{
    vkDestroySwapchainKHR(VkContext::device, handle_, nullptr);

    for (auto image_view : image_views_)
    {
        image_view.destroy();
    }
}

const VkFormat& SwapChain::format() const
{
    return surface_format_.format;
}

const VkExtent2D& SwapChain::extent() const
{
    return extent_;
}

const std::vector<ImageView>& SwapChain::image_views() const
{
    return image_views_;
}

const VkViewport& SwapChain::default_viewport() const
{
    return default_viewport_;
}

const VkRect2D& SwapChain::default_scissor() const
{
    return default_scissor_;
}

size_t SwapChain::size() const
{
    return image_views_.size();
}
