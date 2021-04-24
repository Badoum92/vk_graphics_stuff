#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class Instance
{
public:
    void create();
    void destroy();

    inline VkInstance handle() const
    {
        return handle_;
    }

    inline operator VkInstance() const
    {
        return handle_;
    }

    std::vector<VkPhysicalDevice> get_physical_devices() const;

private:
    VkInstance handle_;
    VkDebugUtilsMessengerEXT debug_handle_;

    void setup_debug_messenger();
};
