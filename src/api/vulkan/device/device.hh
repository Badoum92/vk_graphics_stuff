#pragma once

#include <vulkan/vulkan.h>

class Device
{
public:
    void create();
    void destroy();

    VkQueue graphics_queue() const;
    VkQueue present_queue() const;

    inline VkDevice handle() const
    {
        return handle_;
    }

    inline operator VkDevice() const
    {
        return handle_;
    }

private:
    VkDevice handle_ = VK_NULL_HANDLE;
    VkQueue graphics_queue_;
    VkQueue present_queue_;
};
