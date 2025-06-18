#include "vk/vk_surface.h"

#include "vk/vk_tools.h"
#include "vk/vk_context.h"
#include "vk/vk_image.h"

#include "core/core.h"
#include "core/window.h"

namespace vk
{
static constexpr const char* swapchain_image_names[max_swapchain_images] = {
    "swapchain image 0",
    "swapchain image 1",
    "swapchain image 2",
    "swapchain image 3",
};

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
    ASSERT(false, "Could not find a present queue");
    return queue;
}

surface surface::create(context* context)
{
    surface surface{};
    VkWin32SurfaceCreateInfoKHR create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hwnd = (HWND)context->window->handle;
    create_info.hinstance = GetModuleHandle(nullptr);
    vkCreateWin32SurfaceKHR(context->instance, &create_info, nullptr, &surface.vk_handle);
    surface.present_queue = get_present_queue(context, &surface);
    surface.create_swapchain(context, context->vsync);
    return surface;
}

void surface::destroy(context* context)
{
    destroy_swapchain(context);
    vkDestroySurfaceKHR(context->instance, vk_handle, nullptr);
    vk_handle = VK_NULL_HANDLE;
}

void surface::create_swapchain(context* context, bool vsync)
{
    VkSurfaceCapabilitiesKHR capabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physical_device, vk_handle, &capabilities));
    extent = capabilities.currentExtent;

    uint32_t num_present_modes = 0;
    VK_CHECK(
        vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device, vk_handle, &num_present_modes, nullptr));
    ASSERT(num_present_modes <= 16);
    VkPresentModeKHR present_modes[16];
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(context->physical_device, vk_handle, &num_present_modes,
                                                       present_modes));

    present_mode = VK_PRESENT_MODE_FIFO_KHR;
    if (!vsync)
    {
        for (uint32_t i = 0; i < num_present_modes; ++i)
        {
            if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                present_mode = present_modes[i];
                break;
            }
        }
    }

    VkSurfaceFormatKHR formats[16];
    uint32_t num_formats = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device, vk_handle, &num_formats, nullptr));
    ASSERT(num_formats <= 16);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(context->physical_device, vk_handle, &num_formats, formats));

    format = formats[0];
    for (uint32_t i = 0; i < num_formats; ++i)
    {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            format = formats[i];
            break;
        }
    }

    uint32_t min_image_count = capabilities.minImageCount + 2;
    if (capabilities.maxImageCount > 0 && min_image_count > capabilities.maxImageCount)
    {
        min_image_count = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = vk_handle;
    create_info.minImageCount = min_image_count;
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

    VK_CHECK(vkGetSwapchainImagesKHR(context->device, swapchain, &num_images, nullptr));
    ASSERT(num_images <= max_swapchain_images);
    VkImage vk_images[max_swapchain_images];
    VK_CHECK(vkGetSwapchainImagesKHR(context->device, swapchain, &num_images, vk_images));

    for (uint32_t i = 0; i < num_images; ++i)
    {
        image_description image_desc = {};
        image_desc.width = extent.width;
        image_desc.height = extent.height;
        image_desc.format = format.format;
        image_desc.usage = create_info.imageUsage;
        image_desc.name = swapchain_image_names[i];
        images[i] = context->create_image(image_desc, vk_images[i]);
    }
}

void surface::destroy_swapchain(context* context)
{
    for (uint32_t i = 0; i < num_images; ++i)
    {
        vkDestroyImageView(context->device, images[i]->full_view.vk_handle, nullptr);
        pool_free(&context->images, images[i]);
    }
    vkDestroySwapchainKHR(context->device, swapchain, nullptr);
    swapchain = VK_NULL_HANDLE;
    num_images = 0;
}
} // namespace vk
