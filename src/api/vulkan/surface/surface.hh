#pragma once

#include <vulkan/vulkan.h>

class Surface
{
public:
    void create();
    void destroy();

    inline VkSurfaceKHR handle() const
    {
        return handle_;
    }

    inline operator VkSurfaceKHR() const
    {
        return handle_;
    }

private:
    VkSurfaceKHR handle_ = VK_NULL_HANDLE;
};
