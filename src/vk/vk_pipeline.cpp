#include "vk/vk_pipeline.h"

#include "vk/vk_context.h"
#include "vk/vk_shader.h"
#include "vk/vk_tools.h"

namespace vk
{
graphics_state graphics_state::create()
{
    graphics_state state;
    state.polygon_mode = VK_POLYGON_MODE_FILL;
    state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    state.line_width = 0;
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

graphics_pipeline* context::create_graphics_pipeline(const graphics_pipeline_description& description)
{
    graphics_pipeline* pipeline = pool_alloc(&graphics_pipelines);
    pipeline->description = description;
    pipeline->num_graphics_states = 0;

    VkPushConstantRange push_constant_range = {};
    push_constant_range.stageFlags = VK_SHADER_STAGE_ALL;
    push_constant_range.offset = 0;
    push_constant_range.size = description.push_constant_size;

    VkDescriptorSetLayout layouts[2] = {texture_descriptor_set.layout, image_descriptor_set.layout};

    VkPipelineLayoutCreateInfo layout_create_info = {};
    layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_create_info.setLayoutCount = 2;
    layout_create_info.pSetLayouts = layouts;
    if (push_constant_range.size != 0)
    {
        layout_create_info.pushConstantRangeCount = 1;
        layout_create_info.pPushConstantRanges = &push_constant_range;
    }

    VK_CHECK(vkCreatePipelineLayout(device, &layout_create_info, nullptr, &pipeline->layout));
    set_resource_name(this, (uint64_t)pipeline->layout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, description.name);
    return pipeline;
} // namespace vk

VkPipeline context::compile_graphics_pipeline(graphics_pipeline* pipeline, graphics_state graphics_state)
{
    for (uint32_t i = 0; i < pipeline->num_graphics_states; ++i)
    {
        if (pipeline->graphics_states[i] == graphics_state)
        {
            return pipeline->vk_handles[i];
        }
    }

    ASSERT(pipeline->num_graphics_states < max_graphics_states);

    pipeline->graphics_states[pipeline->num_graphics_states] = graphics_state;

    VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamic_state_info = {};
    dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state_info.dynamicStateCount = ARRAY_SIZE(dynamic_states);
    dynamic_state_info.pDynamicStates = dynamic_states;

    VkPipelineVertexInputStateCreateInfo vertex_input_state_info = {};
    vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineInputAssemblyStateCreateInfo assembly_state_info = {};
    assembly_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assembly_state_info.topology = (VkPrimitiveTopology)graphics_state.topology;
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
    rasterization_state_info.lineWidth = graphics_state.line_width + 1.0f;

    VkPipelineColorBlendAttachmentState color_attachment_states[max_color_attachments];
    uint32_t num_color_attachment_states = 0;
    for (uint32_t i = 0; i < pipeline->description.num_color_formats; ++i)
    {
        VkPipelineColorBlendAttachmentState& attachment_state = color_attachment_states[num_color_attachment_states++];
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
    color_blend_state_info.attachmentCount = num_color_attachment_states;
    color_blend_state_info.pAttachments = color_attachment_states;
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
    vertex_shader_stage_info.module = pipeline->description.vertex_shader->vk_handle;
    vertex_shader_stage_info.pName = "main";
    VkPipelineShaderStageCreateInfo& fragment_shader_stage_info = shader_stages[1];
    fragment_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragment_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragment_shader_stage_info.module = pipeline->description.fragment_shader->vk_handle;
    fragment_shader_stage_info.pName = "main";

    VkPipelineRenderingCreateInfo rendering_create_info = {};
    rendering_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_create_info.colorAttachmentCount = pipeline->description.num_color_formats;
    rendering_create_info.pColorAttachmentFormats = pipeline->description.color_formats;
    rendering_create_info.depthAttachmentFormat = pipeline->description.depth_format;

    VkGraphicsPipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_create_info.flags = VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
    pipeline_create_info.pNext = &rendering_create_info;
    pipeline_create_info.layout = pipeline->layout;
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
    pipeline_create_info.stageCount = ARRAY_SIZE(shader_stages);
    pipeline_create_info.pStages = shader_stages;
    pipeline_create_info.subpass = 0;

    VkPipeline& pipeline_handle = pipeline->vk_handles[pipeline->num_graphics_states++];
    VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline_handle));
    set_resource_name(this, (uint64_t)pipeline_handle, VK_OBJECT_TYPE_PIPELINE, pipeline->description.name);
    return pipeline_handle;
}

void context::destroy_graphics_pipeline(graphics_pipeline* pipeline)
{
    vkDestroyPipelineLayout(device, pipeline->layout, nullptr);
    pipeline->layout = VK_NULL_HANDLE;
    for (uint32_t i = 0; i < pipeline->num_graphics_states; ++i)
    {
        vkDestroyPipeline(device, pipeline->vk_handles[i], nullptr);
    }
    pipeline->num_graphics_states = 0;
}

compute_pipeline* context::create_compute_pipeline(const compute_pipeline_description& description)
{
    compute_pipeline* pipeline = pool_alloc(&compute_pipelines);
    pipeline->description = description;

    VkPushConstantRange push_constant_range = {};
    push_constant_range.stageFlags = VK_SHADER_STAGE_ALL;
    push_constant_range.offset = 0;
    push_constant_range.size = description.push_constant_size;

    VkDescriptorSetLayout layouts[2] = {texture_descriptor_set.layout, image_descriptor_set.layout};

    VkPipelineLayoutCreateInfo layout_create_info = {};
    layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_create_info.setLayoutCount = 2;
    layout_create_info.pSetLayouts = layouts;
    if (push_constant_range.size != 0)
    {
        layout_create_info.pushConstantRangeCount = 1;
        layout_create_info.pPushConstantRanges = &push_constant_range;
    }

    VK_CHECK(vkCreatePipelineLayout(device, &layout_create_info, nullptr, &pipeline->layout));

    VkComputePipelineCreateInfo pipeline_create_info = {};
    pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_create_info.flags = VK_PIPELINE_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
    pipeline_create_info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    pipeline_create_info.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    pipeline_create_info.stage.module = description.shader->vk_handle;
    pipeline_create_info.stage.pName = "main";
    pipeline_create_info.layout = pipeline->layout;

    VK_CHECK(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &pipeline->vk_handle));
    return pipeline;
}

void context::destroy_compute_pipeline(compute_pipeline* pipeline)
{
    vkDestroyPipelineLayout(device, pipeline->layout, nullptr);
    pipeline->layout = VK_NULL_HANDLE;
    vkDestroyPipeline(device, pipeline->vk_handle, nullptr);
    pipeline->vk_handle = VK_NULL_HANDLE;
}
} // namespace vk
