#pragma once

#include <volk.h>

#include <vector>

#include "descriptor_set.h"
#include "shader.h"

namespace vk
{
struct ComputeProgramDescription
{
    bul::Handle<Shader> shader;
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
