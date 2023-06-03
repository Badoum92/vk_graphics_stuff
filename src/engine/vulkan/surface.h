#pragma once

#include <vector>
#include <volk.h>

#include "fwd.h"
#include "bul/containers/handle.h"

namespace vk
{
struct Surface
{
    static Surface create(Context& context, Device& device);
    void destroy(Context& context, Device& device);

    void create_swapchain(Device& device);
    void destroy_swapchain(Device& device);

    VkSurfaceKHR vk_handle = VK_NULL_HANDLE;
    VkQueue present_queue = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkExtent2D extent;
    VkPresentModeKHR present_mode;
    VkSurfaceFormatKHR format;
    std::vector<bul::Handle<Image>> images;
    std::vector<bul::Handle<FrameBuffer>> framebuffers;
};
} // namespace vk
