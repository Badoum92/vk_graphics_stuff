#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "render_pass/render_pass.hh"

class FrameBuffer
{
public:
    void create(const RenderPass& renderpass, const std::vector<VkImageView>& attachments);
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
    VkFramebuffer handle_ = VK_NULL_HANDLE;
};
