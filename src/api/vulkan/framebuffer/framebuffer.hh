#pragma once

#include <vulkan/vulkan.h>

#include "render_pass/render_pass.hh"

class FrameBuffer
{
public:
    void create(const RenderPass& renderpass, const VkImageView& color_attachment);
    void destroy();

    inline VkFramebuffer handle() const
    {
        return handle_;
    }

    inline operator VkFramebuffer() const
    {
        return handle_;
    }

private:
    VkFramebuffer handle_;
};
