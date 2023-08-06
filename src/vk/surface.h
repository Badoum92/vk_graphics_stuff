#pragma once

#include <vector>

#include "bul/containers/handle.h"
#include "vk/image.h"

namespace vk
{
struct Context;

struct Surface
{
    static Surface create(Context* context);
    void destroy();

    void create_swapchain();
    void destroy_swapchain();

    Context* context = nullptr;
    VkSurfaceKHR vk_handle = VK_NULL_HANDLE;
    VkQueue present_queue = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkExtent2D extent;
    VkPresentModeKHR present_mode;
    VkSurfaceFormatKHR format;
    std::vector<bul::Handle<Image>> images;
    // bul::Vector<bul::Handle<FrameBuffer>> framebuffers;
};
} // namespace vk
