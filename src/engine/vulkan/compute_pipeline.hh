#pragma once

#include <volk.h>

#include <vector>

#include "descriptor_set.hh"
#include "shader.hh"

namespace vk
{
struct ComputeProgramDescription
{
    Handle<Shader> shader;
    std::vector<DescriptorType> descriptor_types;
};

struct ComputeProgram
{
    ComputeProgramDescription description;
    VkPipelineLayout layout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    DescriptorSet descriptor_set;
};
} // namespace vk
