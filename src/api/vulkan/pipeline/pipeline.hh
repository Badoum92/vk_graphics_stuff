#pragma once

#include <vulkan/vulkan.h>
#include <string>
#include <optional>

#include "render_pass/render_pass.hh"
#include "shader/shader.hh"

struct PipelineInfo
{
    PipelineInfo(const std::string& vertex, const std::string& fragment);

    PipelineInfo& set_default();
    PipelineInfo& set_topology(VkPrimitiveTopology topology, bool restart_enable = false);
    PipelineInfo& set_viewport_scissor(const VkViewport& viewport, const VkRect2D &scissor);
    PipelineInfo& set_polygon_mode(VkPolygonMode mode);
    PipelineInfo& set_cull_mode(VkCullModeFlags cull_mode, VkFrontFace front_face);
    PipelineInfo& set_line_width(float width);
    // TODO: multisampling
    PipelineInfo& set_depth_test(bool test);
    PipelineInfo& set_dynamic_states(const std::vector<VkDynamicState>& states);
    PipelineInfo& set_renderpass(const RenderPass& pass);

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_info;
    VkPipelineViewportStateCreateInfo viewport_state_info;
    VkPipelineRasterizationStateCreateInfo rasterizer_info;
    VkPipelineMultisampleStateCreateInfo multisampling_info;
    VkPipelineDepthStencilStateCreateInfo depth_stencil;
    VkPipelineColorBlendAttachmentState color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo color_blend_info;
    std::vector<VkDynamicState> dynamic_states;
    VkPipelineDynamicStateCreateInfo dynamic_state_info;
    VkRenderPass renderpass;

    std::optional<std::string> vertex_shader;
    std::optional<std::string> fragment_shader;
    std::optional<std::string> compute_shader;
};

class Pipeline
{
public:
    Pipeline() = default;
    // Pipeline(const std::string& vertex, const std::string& fragment);
    ~Pipeline();

    // void create(const std::string& vertex, const std::string& fragment);
    void create(const PipelineInfo& info);
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
    VkPipeline handle_ = VK_NULL_HANDLE;
    VkPipelineLayout layout_;
    Shader shader_;

    VkPipelineBindPoint bind_point_;
};
