#pragma once

#include <vector>

#include "bul/containers/handle.h"
#include "vk/image.h"

namespace vk
{
struct context;

struct surface
{
    static surface create(context* context);
    void destroy();

    void create_swapchain();
    void destroy_swapchain();

    context* context = nullptr;
    VkSurfaceKHR vk_handle = VK_NULL_HANDLE;
    VkQueue present_queue = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkExtent2D extent;
    VkPresentModeKHR present_mode;
    VkSurfaceFormatKHR format;
    std::vector<bul::handle<image>> images;
    // bul::Vector<bul::handle<FrameBuffer>> framebuffers;
};
} // namespace vk
