#pragma once

#include <vulkan/vulkan.h>
#include <string>

#include "shader/shader.hh"

class Pipeline
{
public:
    Pipeline() = default;
    Pipeline(const std::string& vertex, const std::string& fragment);
    ~Pipeline();

    void create(const std::string& vertex, const std::string& fragment);
    void destroy();

    VkPipelineBindPoint bind_point() const;
    VkPipelineLayout layout() const;
    DescriptorSet& descriptor_set(size_t n);
    const DescriptorSet& descriptor_set(size_t n) const;

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
