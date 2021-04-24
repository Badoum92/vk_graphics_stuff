#pragma once

#include <vulkan/vulkan.h>

#include "render_pass/render_pass.hh"

class Pipeline
{
public:
    void create();
    void destroy();

    inline VkPipeline handle() const
    {
        return handle_;
    }

    inline operator VkPipeline() const
    {
        return handle_;
    }

private:
    VkPipeline handle_;
    VkPipelineLayout layout_;
};
