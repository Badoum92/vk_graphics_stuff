#pragma once

#include <vector>
#include <volk.h>

#include "fwd.h"
#include "bul/containers/handle.h"

namespace vk
{
namespace surface
{
    void create();
    void destroy();

    void create_swapchain();
    void destroy_swapchain();

    inline VkSurfaceKHR vk_handle = VK_NULL_HANDLE;
    inline VkQueue present_queue = VK_NULL_HANDLE;
    inline VkSwapchainKHR swapchain = VK_NULL_HANDLE;
    inline VkExtent2D extent;
    inline VkPresentModeKHR present_mode;
    inline VkSurfaceFormatKHR format;
    inline std::vector<bul::handle<Image>> images;
};
} // namespace vk
