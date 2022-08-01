#pragma once

#include <volk.h>

namespace vk
{
struct PhysicalDevice
{
    VkPhysicalDevice vk_handle = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties properties;
};

struct Context
{
    static Context create();
    void destroy();

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    PhysicalDevice physical_device;
};
} // namespace vk
