#pragma once

#include <vulkan/vulkan.h>

class CommandPool
{
public:
    void create();
    void destroy();

    inline VkCommandPool handle() const
    {
        return handle_;
    }

    inline operator VkCommandPool() const
    {
        return handle_;
    }

private:
    VkCommandPool handle_;
};
