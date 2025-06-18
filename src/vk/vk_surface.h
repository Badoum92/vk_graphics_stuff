#pragma once

#include <volk.h>

namespace vk
{
struct context;
struct image;

constexpr uint32_t max_swapchain_images = 4;

struct surface
{
    static surface create(context* context);
    void destroy(context* context);

    void create_swapchain(context* context, bool vsync);
    void destroy_swapchain(context* context);

    VkSurfaceKHR vk_handle = VK_NULL_HANDLE;
    VkQueue present_queue = VK_NULL_HANDLE;
    VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    VkExtent2D extent;
    VkPresentModeKHR present_mode;
    VkSurfaceFormatKHR format;
    image* images[max_swapchain_images];
    uint32_t num_images;
};
} // namespace vk
