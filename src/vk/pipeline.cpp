#include "vk/pipeline.h"

#include "vk/context.h"

namespace vk
{
graphics_state graphics_state::create()
{
    graphics_state state;
    state.polygon_mode = VK_POLYGON_MODE_FILL;
    state.cull_back_faces = true;
    state.front_face_clockwise = true;
    state.depth_test_enabled = true;
    state.depth_write_enabled = true;
    state.depth_compare_op = VK_COMPARE_OP_GREATER_OR_EQUAL;
    return state;
}

bool operator==(const graphics_state& a, const graphics_state& b)
{
    return memcmp(&a, &b, sizeof(graphics_state)) == 0;
}

bul::handle<graphics_pipeline> context::create_graphics_pipeline(const graphics_pipeline_description& description)
{
    graphics_pipeline graphics_pipeline = {};
    graphics_pipeline.description = description;

    VkPushConstantRange push_constant_range = {};
    push_constant_range.stageFlags = VK_SHADER_STAGE_ALL;
    push_constant_range.offset = 0;
    push_constant_range.size = description.push_constant_size;

    VkPipelineLayoutCreateInfo layout_create_info = {};
    layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_create_info.setLayoutCount = 1;
    layout_create_info.pSetLayouts = &descriptor_set.layout;
    if (push_constant_range.size != 0)
    {
        layout_create_info.pushConstantRangeCount = 1;
        layout_create_info.pPushConstantRanges = &push_constant_range;
    }

    VK_CHECK(vkCreatePipelineLayout(device, &layout_create_info, nullptr, &graphics_pipeline.layout));

    set_resource_name(this, (uint64_t)graphics_pipeline.layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, description.name);

    return graphics_pipelines.insert(graphics_pipeline);
}

VkPipeline context::compile_graphics_pipeline(bul::handle<graphics_pipeline> handle, graphics_state graphics_state)
{
    graphics_pipeline& graphics_pipeline = graphics_pipelines.get(handle);

    for (uint32_t i = 0; i < graphics_pipeline.graphics_states.size; ++i)
    {
        if (graphics_pipeline.graphics_states[i] == graphics_state)
        {
            return graphics_pipeline.pipelines[i];
        }
    }

    graphics_pipeline.graphics_states.push_back(graphics_state);

    VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = BUL_ARRAY_SIZE(dynamic_states);
    dynamic_state_info.pDynamicStates = dynamic_states;

    VkPipelineVertexInputStateCreateInfo vertex_input_state_info = {};
    vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo assembly_state_info = {};
    assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembly_state_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assembly_state_info.primitiveRestartEnable = false;

    VkPipelineRasterizationStateCreateInfo rasterization_state_info = {};
    rasterization_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_state_info.depthClampEnable = false;
    rasterization_state_info.rasterizerDiscardEnable = false;
    rasterization_state_info.polygonMode = (VkPolygonMode)graphics_state.polygon_mode;
    rasterization_state_info.cullMode = graphics_state.cull_back_faces ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
    rasterization_state_info.frontFace =
        (VkFrontFace)graphics_state.front_face_clockwise ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_state_info.depthBiasEnable = false;
    rasterization_state_info.depthBiasConstantFactor = 0;
    rasterization_state_info.depthBiasClamp = 0;
    rasterization_state_info.depthBiasSlopeFactor = 0;
    rasterization_state_info.lineWidth = 1.0f;

    bul::static_vector<VkPipelineColorBlendAttachmentState, max_color_attachments> color_attachment_states;
    for (uint32_t i = 0; i < graphics_pipeline.description.color_attachment_formats.size; ++i)
    {
        VkPipelineColorBlendAttachmentState& attachment_state = color_attachment_states.push_back();
        attachment_state.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        attachment_state.blendEnable = false;
        attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
        attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;
    }

    VkPipelineColorBlendStateCreateInfo color_blend_state_info = {};
    color_blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state_info.flags = 0;
    color_blend_state_info.attachmentCount = color_attachment_states.size;
    color_blend_state_info.pAttachments = color_attachment_states.data;
    color_blend_state_info.logicOpEnable = false;
    color_blend_state_info.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state_info.blendConstants[0] = 0.0f;
    color_blend_state_info.blendConstants[1] = 0.0f;
    color_blend_state_info.blendConstants[2] = 0.0f;
    color_blend_state_info.blendConstants[3] = 0.0f;

    VkPipelineViewportStateCreateInfo viewport_state_info = {};
    viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state_info.flags = 0;
    viewport_state_info.viewportCount = 1;
    viewport_state_info.scissorCount = 1;
    viewport_state_info.pScissors = nullptr;
    viewport_state_info.pViewports = nullptr;

    VkPipelineDepthStencilStateCreateInfo depth_state_info = {};
    depth_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_state_info.flags = 0;
    depth_state_info.depthTestEnable = graphics_state.depth_test_enabled;
    depth_state_info.depthWriteEnable = graphics_state.depth_write_enabled;
    depth_state_info.depthCompareOp = (VkCompareOp)graphics_state.depth_compare_op;
    depth_state_info.depthBoundsTestEnable = false;
    depth_state_info.minDepthBounds = 0.0f;
    depth_state_info.maxDepthBounds = 0.0f;
    depth_state_info.stencilTestEnable = false;
    depth_state_info.back.failOp = VK_STENCIL_OP_KEEP;
    depth_state_info.back.passOp = VK_STENCIL_OP_KEEP;
    depth_state_info.back.compareOp = VK_COMPARE_OP_ALWAYS;
    depth_state_info.back.compareMask = 0;
    depth_state_info.back.reference = 0;
    depth_state_info.back.depthFailOp = VK_STENCIL_OP_KEEP;
    depth_state_info.back.writeMask = 0;
    depth_state_info.front = depth_state_info.back;

    VkPipelineMultisampleStateCreateInfo multisampling_state_info = {};
    multisampling_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling_state_info.flags = 0;
    multisampling_state_info.sampleShadingEnable = false;
    multisampling_state_info.alphaToCoverageEnable = false;
    multisampling_state_info.alphaToOneEnable = false;
    multisampling_state_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling_state_info.minSampleShading = 1.0f;
    multisampling_state_info.pSampleMask = nullptr;

    VkPipelineShaderStageCreateInfo shader_stages[2] = {};
    VkPipelineShaderStageCreateInfo& vertex_shader_stage_info = shader_stages[0];
    vertex_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertex_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertex_shader_stage_info.module = shaders.get(graphics_pipeline.description.vertex_shader).vk_handle;
    vertex_shader_stage_info.pName = "main";
    VkPipelineShaderStageCreateInfo& fragment_shader_stage_info = shader_stages[1];
    fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_stage_info.module = shaders.get(graphics_pipeline.description.fragment_shader).vk_handle;
    fragment_shader_stage_info.pName = "main";

    VkPipelineRenderingCreateInfo rendering_create_info = {};
    rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_create_info.colorAttachmentCount = graphics_pipeline.description.color_attachment_formats.size;
    rendering_create_info.pColorAttachmentFormats = graphics_pipeline.description.color_attachment_formats.data;
    rendering_create_info.depthAttachmentFormat = graphics_pipeline.description.depth_attachment_format;

    VkGraphicsPipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.flags = VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
    pipeline_create_info.pNext = &rendering_create_info;
    pipeline_create_info.layout = graphics_pipeline.layout;
    pipeline_create_info.basePipelineHandle = nullptr;
    pipeline_create_info.basePipelineIndex = 0;
    pipeline_create_info.pVertexInputState = &vertex_input_state_info;
    pipeline_create_info.pInputAssemblyState = &assembly_state_info;
    pipeline_create_info.pRasterizationState = &rasterization_state_info;
    pipeline_create_info.pColorBlendState = &color_blend_state_info;
    pipeline_create_info.pTessellationState = nullptr;
    pipeline_create_info.pMultisampleState = &multisampling_state_info;
    pipeline_create_info.pDynamicState = &dynamic_state_info;
    pipeline_create_info.pViewportState = &viewport_state_info;
    pipeline_create_info.pDepthStencilState = &depth_state_info;
    pipeline_create_info.stageCount = BUL_ARRAY_SIZE(shader_stages);
    pipeline_create_info.pStages = shader_stages;
    pipeline_create_info.subpass = 0;

    VkPipeline& pipeline = graphics_pipeline.pipelines.push_back();
    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline));
    set_resource_name(this, (uint64_t)pipeline, VK_OBJECT_TYPE_PIPELINE, graphics_pipeline.description.name);
    return pipeline;
}

void context::destroy_graphics_pipeline(bul::handle<graphics_pipeline> handle)
{
    graphics_pipeline& graphics_pipeline = graphics_pipelines.get(handle);
    vkDestroyPipelineLayout(device, graphics_pipeline.layout, nullptr);
    graphics_pipeline.layout = VK_NULL_HANDLE;
    for (VkPipeline pipeline : graphics_pipeline.pipelines)
    {
        vkDestroyPipeline(device, pipeline, nullptr);
    }
    graphics_pipeline.pipelines.clear();
    graphics_pipeline.graphics_states.clear();
}
} // namespace vk