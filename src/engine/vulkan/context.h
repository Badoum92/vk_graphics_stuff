/* #pragma once

#include <volk.h>

namespace vk
{
struct PhysicalDevice
{
    VkPhysicalDevice vk_handle = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties properties;
};

namespace context
{
    void create();
    void destroy();

    inline VkInstance instance = VK_NULL_HANDLE;
    inline VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;
    inline PhysicalDevice physical_device;
};
} // namespace vk
 */
