#pragma once

#include <vulkan/vulkan.h>

#include "shader/shader.hh"

class Pipeline
{
public:
    void create();
    void destroy();

    void bind();
    void bind_set(size_t n);
    VkPipelineBindPoint bind_point() const;

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
    Shader shader_;

    VkPipelineBindPoint bind_point_;
};
