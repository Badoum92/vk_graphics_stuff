#include "graphics_pipeline.h"

#include <array>

#include "vk_tools.h"
#include "device.h"

namespace vk
{
bul::Handle<GraphicsProgram> device::create_graphics_program(const GraphicsProgramDescription& description)
{
    DescriptorSet set = create_descriptor_set(description.descriptor_types);

    GraphicsProgram program{};
    program.description = description;
    program.descriptor_set = set;

    std::array sets{global_uniform_set.layout, set.layout};

    VkPipelineLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = sets.size();
    layout_info.pSetLayouts = sets.data();

    VK_CHECK(vkCreatePipelineLayout(vk_handle, &layout_info, nullptr, &program.layout));

    return graphics_programs.insert(std::move(program));
}

void device::destroy_graphics_program(GraphicsProgram& graphics_program)
{
    destroy_descriptor_set(graphics_program.descriptor_set);
    vkDestroyPipelineLayout(vk_handle, graphics_program.layout, nullptr);
    graphics_program.layout = VK_NULL_HANDLE;
    for (auto& pipeline : graphics_program.pipelines)
    {
        vkDestroyPipeline(vk_handle, pipeline, nullptr);
    }
    graphics_program.pipelines.clear();
}

VkPipeline device::compile(const bul::Handle<GraphicsProgram>& handle, const RenderState& render_state)
{
    GraphicsProgram& program = graphics_programs.get(handle);

    std::vector<VkDynamicState> dynamic_states = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state_info{};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = dynamic_states.size();
    dynamic_state_info.pDynamicStates = dynamic_states.data();

    VkPipelineVertexInputStateCreateInfo vertex_input_state_info{};
    vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo assembly_state_info{};
    assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembly_state_info.flags = 0;
    assembly_state_info.topology = render_state.input_assembly.topology;
    assembly_state_info.primitiveRestartEnable = VK_FALSE;

    VkPipelineRasterizationStateCreateInfo rasterization_state_info{};
    rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_info.flags = 0;
    rasterization_state_info.depthClampEnable = VK_FALSE;
    rasterization_state_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_state_info.polygonMode = render_state.rasterization.polygon_mode;
    rasterization_state_info.cullMode = render_state.rasterization.cull_mode;
    rasterization_state_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state_info.depthBiasEnable = render_state.depth.bias != 0.0f;
    rasterization_state_info.depthBiasConstantFactor = render_state.depth.bias;
    rasterization_state_info.depthBiasClamp = 0;
    rasterization_state_info.depthBiasSlopeFactor = 0;
    rasterization_state_info.lineWidth = render_state.rasterization.line_width;

    std::vector<VkPipelineColorBlendAttachmentState> attachment_states;
    attachment_states.reserve(program.description.attachment_formats.color_formats.size());

    for (size_t i = 0; i < program.description.attachment_formats.color_formats.size(); ++i)
    {
        attachment_states.emplace_back();
        auto& attachment_state = attachment_states.back();

        attachment_state.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        attachment_state.blendEnable = VK_FALSE;
        attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
        attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
    }

    VkPipelineColorBlendStateCreateInfo color_blend_state_info{};
    color_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state_info.flags = 0;
    color_blend_state_info.attachmentCount = attachment_states.size();
    color_blend_state_info.pAttachments = attachment_states.data();
    color_blend_state_info.logicOpEnable = VK_FALSE;
    color_blend_state_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state_info.blendConstants[0] = 0.0f;
    color_blend_state_info.blendConstants[1] = 0.0f;
    color_blend_state_info.blendConstants[2] = 0.0f;
    color_blend_state_info.blendConstants[3] = 0.0f;

    VkPipelineViewportStateCreateInfo viewport_state_info{};
    viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_info.flags = 0;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.scissorCount = 1;
    viewport_state_info.pScissors = nullptr;
    viewport_state_info.pViewports = nullptr;

    VkPipelineDepthStencilStateCreateInfo depth_state_info{};
    depth_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_state_info.flags = 0;
    depth_state_info.depthTestEnable = render_state.depth.test_enable;
    depth_state_info.depthWriteEnable = render_state.depth.write_enable;
    depth_state_info.depthCompareOp =
        render_state.depth.test_enable ? render_state.depth.compare_op : VK_COMPARE_OP_NEVER;
    depth_state_info.depthBoundsTestEnable = VK_FALSE;
    depth_state_info.minDepthBounds = 0.0f;
    depth_state_info.maxDepthBounds = 0.0f;
    depth_state_info.stencilTestEnable = VK_FALSE;
    depth_state_info.back.failOp = VK_STENCIL_OP_KEEP;
    depth_state_info.back.passOp = VK_STENCIL_OP_KEEP;
    depth_state_info.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_state_info.back.compareMask = 0;
    depth_state_info.back.reference = 0;
    depth_state_info.back.depthFailOp = VK_STENCIL_OP_KEEP;
    depth_state_info.back.writeMask = 0;
    depth_state_info.front = depth_state_info.back;

    VkPipelineMultisampleStateCreateInfo multisampling_state_info{};
    multisampling_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_state_info.flags = 0;
    multisampling_state_info.sampleShadingEnable = VK_FALSE;
    multisampling_state_info.alphaToCoverageEnable = VK_FALSE;
    multisampling_state_info.alphaToOneEnable = VK_FALSE;
    multisampling_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_state_info.minSampleShading = 1.0f;
    multisampling_state_info.pSampleMask = nullptr;

    std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
    {
        auto& shader = shaders.get(program.description.vertex_shader);
        VkPipelineShaderStageCreateInfo shader_stage_info{};
        shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
        shader_stage_info.module = shader.vk_handle;
        shader_stage_info.pName = "main";
        shader_stages.push_back(shader_stage_info);
    }
    {
        auto& shader = shaders.get(program.description.fragment_shader);
        VkPipelineShaderStageCreateInfo shader_stage_info{};
        shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shader_stage_info.module = shader.vk_handle;
        shader_stage_info.pName = "main";
        shader_stages.push_back(shader_stage_info);
    }

    VkPipelineRenderingCreateInfo rendering_create_info{};
    rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_create_info.colorAttachmentCount = program.description.attachment_formats.color_formats.size();
    rendering_create_info.pColorAttachmentFormats = program.description.attachment_formats.color_formats.data();
    rendering_create_info.depthAttachmentFormat =
        program.description.attachment_formats.depth_format.value_or(VK_FORMAT_UNDEFINED);

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext = &rendering_create_info;
    pipeline_info.layout = program.layout;
    pipeline_info.basePipelineHandle = nullptr;
    pipeline_info.basePipelineIndex = 0;
    pipeline_info.pVertexInputState = &vertex_input_state_info;
    pipeline_info.pInputAssemblyState = &assembly_state_info;
    pipeline_info.pRasterizationState = &rasterization_state_info;
    pipeline_info.pColorBlendState = &color_blend_state_info;
    pipeline_info.pTessellationState = nullptr;
    pipeline_info.pMultisampleState = &multisampling_state_info;
    pipeline_info.pDynamicState = &dynamic_state_info;
    pipeline_info.pViewportState = &viewport_state_info;
    pipeline_info.pDepthStencilState = &depth_state_info;
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.stageCount = shader_stages.size();
    pipeline_info.subpass = 0;

    VkPipeline vk_pipeline = VK_NULL_HANDLE;
    VK_CHECK(vkCreateGraphicsPipelines(vk_handle, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &vk_pipeline));

    program.pipelines.push_back(vk_pipeline);
    program.render_states.push_back(render_state);

    return vk_pipeline;
}
} // namespace vk
