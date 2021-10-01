#include "surface.hh"

#include <vector>
#include <GLFW/glfw3.h>

#include "vk_tools.hh"
#include "context.hh"
#include "device.hh"
#include "image.hh"
#include "window.hh"

namespace vk
{
static VkQueue get_present_queue(Device& device, const Surface& surface)
{
    VkQueue queue = VK_NULL_HANDLE;
    VkBool32 supported = false;
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device.physical_device.vk_handle, device.graphics_family_index,
                                                  surface.vk_handle, &supported));
    if (supported)
    {
        vkGetDeviceQueue(device.vk_handle, device.graphics_family_index, 0, &queue);
        return queue;
    }
    VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(device.physical_device.vk_handle, device.compute_family_index,
                                                  surface.vk_handle, &supported));
    if (supported)
    {
        vkGetDeviceQueue(device.vk_handle, device.compute_family_index, 0, &queue);
        return queue;
    }
    throw std::runtime_error("Could not find a present queue");
}

Surface Surface::create(Context& context, Device& device)
{
    Surface surface{};
    VK_CHECK(glfwCreateWindowSurface(context.instance, Window::handle(), nullptr, &surface.vk_handle));
    surface.present_queue = get_present_queue(device, surface);
    surface.create_swapchain(device);
    return surface;
}

void Surface::destroy(Context& context, Device& device)
{
    destroy_swapchain(device);
    vkDestroySurfaceKHR(context.instance, vk_handle, nullptr);
    vk_handle = VK_NULL_HANDLE;
}

void Surface::create_swapchain(Device& device)
{
    VkSurfaceCapabilitiesKHR capabilities;
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical_device.vk_handle, vk_handle, &capabilities));
    extent = capabilities.currentExtent;

    uint32_t present_mode_count = 0;
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical_device.vk_handle, vk_handle, &present_mode_count,
                                                       nullptr));
    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical_device.vk_handle, vk_handle, &present_mode_count,
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
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical_device.vk_handle, vk_handle, &format_count, nullptr));
    formats.resize(format_count);
    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical_device.vk_handle, vk_handle, &format_count,
                                                  formats.data()));

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
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 0;
    create_info.pQueueFamilyIndices = nullptr;
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(device.vk_handle, &create_info, nullptr, &swapchain));

    image_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(device.vk_handle, swapchain, &image_count, nullptr));
    std::vector<VkImage> vk_images(image_count);
    VK_CHECK(vkGetSwapchainImagesKHR(device.vk_handle, swapchain, &image_count, vk_images.data()));

    images.resize(image_count);
    framebuffers.resize(image_count);

    ImageDescription img_desc{};
    img_desc.width = extent.width;
    img_desc.height = extent.height;
    img_desc.format = format.format;
    img_desc.usage = create_info.imageUsage;

    FrameBufferDescription fb_desc{};
    fb_desc.color_formats = {format.format};
    fb_desc.width = extent.width;
    fb_desc.height = extent.height;

    for (uint32_t i = 0; i < image_count; ++i)
    {
        images[i] = device.create_image(img_desc, vk_images[i]);
        framebuffers[i] = device.create_framebuffer(fb_desc, {images[i]}, Handle<Image>::invalid());
    }
}

void Surface::destroy_swapchain(Device& device)
{
    for (uint32_t i = 0; i < images.size(); ++i)
    {
        device.destroy_image(images[i]);
        device.destroy_framebuffer(framebuffers[i]);
    }
    vkDestroySwapchainKHR(device.vk_handle, swapchain, nullptr);
    swapchain = VK_NULL_HANDLE;
}
} // namespace vk
