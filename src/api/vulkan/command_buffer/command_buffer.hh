#pragma once

#include <vulkan/vulkan.h>
#include <functional>

#include "framebuffer/framebuffer.hh"
#include "render_pass/render_pass.hh"
#include "buffer/buffer.hh"
#include "pipeline/pipeline.hh"

class CommandBuffer
{
public:
    void create(VkCommandPool pool);
    void destroy();

    void begin();
    void end();

    void begin_renderpass(const RenderPass& renderpass, const FrameBuffer& framebuffer);
    void end_renderpass();

    void draw(uint32_t vertex_count);
    void draw_indexed(uint32_t index_count, uint32_t index_offset = 0);

    void bind_vertex_buffer(const Buffer& buffer, size_t offset = 0);
    void bind_index_buffer(const Buffer& buffer, size_t offset = 0, VkIndexType index_type = VK_INDEX_TYPE_UINT32);
    void bind_pipeline(const Pipeline& pipeline);
    void bind_descriptor_set(const Pipeline& pipeline, uint32_t set_index);

    void set_viewport(const VkViewport& viewport);
    void set_scissor(const VkRect2D& scissor);

    inline const VkCommandBuffer& handle() const
    {
        return handle_;
    }

    inline operator const VkCommandBuffer&() const
    {
        return handle_;
    }

private:
    VkCommandBuffer handle_;
    VkCommandPool pool_;
};
