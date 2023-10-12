#include "compute_pipeline.h"

#include <array>

#include "device.h"

namespace vk
{
bul::handle<ComputeProgram> device::create_compute_program(const ComputeProgramDescription& description)
{
    DescriptorSet set = create_descriptor_set(description.descriptor_types);

    ComputeProgram program{};
    program.description = description;
    program.descriptor_set = set;

    std::array sets{global_uniform_set.layout, set.layout};

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = sets.size();
    layout_info.pSetLayouts = sets.data();

    VK_CHECK(vkCreatePipelineLayout(vk_handle, &layout_info, nullptr, &program.layout));

    VkComputePipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_info.stage.module = shaders.get(description.shader).vk_handle;
    pipeline_info.stage.pName = "main";
    pipeline_info.layout = program.layout;

    VK_CHECK(vkCreateComputePipelines(vk_handle, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &program.pipeline));

    return compute_programs.insert(std::move(program));
}

void device::destroy_compute_program(ComputeProgram& compute_program)
{
    vkDestroyPipeline(vk_handle, compute_program.pipeline, nullptr);
    vkDestroyPipelineLayout(vk_handle, compute_program.layout, nullptr);
    destroy_descriptor_set(compute_program.descriptor_set);

    compute_program.layout = VK_NULL_HANDLE;
    compute_program.pipeline = VK_NULL_HANDLE;
}
} // namespace vk
