#pragma once

#include <vulkan/vulkan.h>

class RenderPass
{
public:
    void create(uint32_t colors, bool depth, bool present);
    void destroy();

    uint32_t colors() const;
    bool depth() const;

    inline VkRenderPass handle() const
    {
        return handle_;
    }

    inline operator VkRenderPass() const
    {
        return handle_;
    }

private:
    VkRenderPass handle_ = VK_NULL_HANDLE;
    uint32_t colors_;
    bool depth_;
};
