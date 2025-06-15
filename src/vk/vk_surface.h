#pragma once

#include "vk/vk_image.h"

#include "core/containers/handle.h"

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
    bul::handle<image> images[max_swapchain_images];
    uint32_t num_images;
};
} // namespace vk
