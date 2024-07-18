#pragma once

#include "vk/image.h"

#include "bul/containers/handle.h"
#include "bul/containers/static_vector.h"

namespace vk
{
struct context;

constexpr uint32_t max_swapchain_images = 4;

struct surface
{
    static surface create(context* context);
    void destroy(context* context);

    void create_swapchain(context* context);
    void destroy_swapchain(context* context);

    VkSurfaceKHR vk_handle = VK_NULL_HANDLE;
    VkQueue present_queue = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkExtent2D extent;
    VkPresentModeKHR present_mode;
    VkSurfaceFormatKHR format;
    bul::static_vector<bul::handle<image>, max_swapchain_images> images;
};
} // namespace vk
