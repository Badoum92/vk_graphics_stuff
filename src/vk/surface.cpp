#include "surface.h"

#include <vector>

#include "bul/bul.h"
#include "bul/format.h"
#include "bul/window.h"
#include "bul/containers/handle.h"
#include "vk_tools.h"
#include "vk/context.h"
#include "vk/image.h"

namespace vk
{
static VkQueue get_present_queue(context* context, surface* surface)
{
    VkQueue queue = VK_NULL_HANDLE;
    VkBool32 supported = false;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(context->physical_device, context->graphics_queue_index,
                                                  surface->vk_handle, &supported));
    if (supported)
    {
        vkGetDeviceQueue(context->device, context->graphics_queue_index, 0, &queue);
        return queue;
    }
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(context->physical_device, context->compute_queue_index,
                                                  surface->vk_handle, &supported));
    if (supported)
    {
        vkGetDeviceQueue(context->device, context->compute_queue_index, 0, &queue);
        return queue;
    }
    ASSERT_FMT(false, "Could not find a present queue");
    return queue;
}

surface surface::create(context* context)
{
    surface surface{};
    surface.context = context;
    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hwnd = reinterpret_cast<HWND>(bul::window::handle());
    create_info.hinstance = GetModuleHandle(nullptr);
    vkCreateWin32SurfaceKHR(context->instance, &create_info, nullptr, &surface.vk_handle);
    surface.present_queue = get_present_queue(context, &surface);
    surface.create_swapchain();
    return surface;
}

void surface::destroy()
{
    destroy_swapchain();
    vkDestroySurfaceKHR(context->instance, vk_handle, nullptr);
    vk_handle = VK_NULL_HANDLE;
    context = nullptr;
}

void surface::create_swapchain()
{
    VkSurfaceCapabilitiesKHR capabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physical_device, vk_handle, &capabilities));
    extent = capabilities.currentExtent;

    uint32_t present_mode_count = 0;
    VK_CHECK(
        vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device, vk_handle, &present_mode_count, nullptr));
    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device, vk_handle, &present_mode_count,
                                                       present_modes.data()));

    present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& mode : present_modes)
    {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            present_mode = mode;
            break;
        }
    }

    uint32_t format_count = 0;
    std::vector<VkSurfaceFormatKHR> formats;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device, vk_handle, &format_count, nullptr));
    formats.resize(format_count);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device, vk_handle, &format_count, formats.data()));

    format = formats[0];
    for (const auto& surface_format : formats)
    {
        if (surface_format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            format = surface_format;
            break;
        }
    }

    uint32_t image_count = capabilities.minImageCount + 2;
    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount)
    {
        image_count = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = vk_handle;
    create_info.minImageCount = image_count;
    create_info.imageFormat = format.format;
    create_info.imageColorSpace = format.colorSpace;
    create_info.imageExtent = extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = image_usage_color_attachment;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(context->device, &create_info, nullptr, &swapchain));

    image_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(context->device, swapchain, &image_count, nullptr));
    std::vector<VkImage> vk_images(image_count);
    VK_CHECK(vkGetSwapchainImagesKHR(context->device, swapchain, &image_count, vk_images.data()));

    images.resize(image_count);

    for (size_t i = 0; i < image_count; ++i)
    {
        image_description image_desc{};
        image_desc.width = extent.width;
        image_desc.height = extent.height;
        image_desc.format = format.format;
        image_desc.usage = create_info.imageUsage;
        image_desc.name = bul::format("Swapchain image #%zu\n", i);
        images[i] = context->create_image(std::move(image_desc), vk_images[i]);
    }
}

void surface::destroy_swapchain()
{
    for (const auto& image : images)
    {
        context->destroy_image(image);
    }
    vkDestroySwapchainKHR(context->device, swapchain, nullptr);
    swapchain = VK_NULL_HANDLE;
}
} // namespace vk
