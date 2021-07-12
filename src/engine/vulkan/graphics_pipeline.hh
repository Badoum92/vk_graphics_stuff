#pragma once

#include <vulkan/vulkan.h>

#include "framebuffer.hh"
#include "descriptor_set.hh"
#include "shader.hh"

namespace vk
{
struct InputAssemblyState
{
    VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
};

struct RasterizationState
{
    VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL;
    VkCullModeFlags cull_mode = VK_CULL_MODE_NONE;
    VkFrontFace front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    float line_width = 1.0f;
};

struct DepthState
{
    VkBool32 test_enable = VK_TRUE;
    VkBool32 write_enable = VK_TRUE;
    VkCompareOp compare_op = VK_COMPARE_OP_GREATER_OR_EQUAL;
    float bias = 0.0f;
};

struct RenderState
{
    InputAssemblyState input_assembly;
    RasterizationState rasterization;
    DepthState depth;
};

struct GraphicsProgramDescription
{
    Handle<Shader> vertex_shader;
    Handle<Shader> fragment_shader;
    std::vector<DescriptorType> descriptor_types;
    FrameBufferDescription attachment_formats;
};

struct GraphicsProgram
{
    GraphicsProgramDescription description;
    VkPipelineLayout layout = VK_NULL_HANDLE;
    std::vector<VkPipeline> pipelines;
    std::vector<RenderState> render_states;
    RenderPass renderpass;
    DescriptorSet descriptor_set;
};
} // namespace vk
