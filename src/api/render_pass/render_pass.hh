#pragma once

#include <vulkan/vulkan.h>

class RenderPass
{
public:
    void create();
    void destroy();

    inline VkRenderPass handle() const
    {
        return handle_;
    }

    inline operator VkRenderPass() const
    {
        return handle_;
    }

private:
    VkRenderPass handle_;
};
