#pragma once

#include <vulkan/vulkan.h>

class DescriptorPool
{
public:
    void create();
    void destroy();

    inline VkDescriptorPool handle() const
    {
        return handle_;
    }

    inline operator VkDescriptorPool() const
    {
        return handle_;
    }

private:
    VkDescriptorPool handle_ = VK_NULL_HANDLE;
};
