#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "handle.hh"

namespace vk
{
struct Context;
struct Device;
struct Image;
struct FrameBuffer;

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
    std::vector<Handle<Image>> images;
    std::vector<Handle<FrameBuffer>> framebuffers;
};
} // namespace vk
