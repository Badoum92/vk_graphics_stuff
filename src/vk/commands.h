#pragma once

#include <volk.h>

#include "vk/image.h"
#include "vk/buffer.h"
#include "vk/pipeline.h"

#include "bul/containers/handle.h"
#include "bul/containers/vector.h"

namespace vk
{
struct context;

struct command_buffer
{
    /* void begin();
    void end();

    void barrier(const bul::handle<Image>& image_handle, ImageUsage dst_usage);

    void bind_image(const bul::handle<GraphicsProgram>& program_handle, const bul::handle<Image>& image_handle,
                    uint32_t binding);
    void bind_uniform_buffer(const bul::handle<GraphicsProgram>& program_handle,
                             const bul::handle<Buffer>& buffer_handle, uint32_t binding, uint32_t offset,
                             uint32_t size);
    void bind_storage_buffer(const bul::handle<GraphicsProgram>& program_handle,
                             const bul::handle<Buffer>& buffer_handle, uint32_t binding);
    void bind_image(const bul::handle<ComputeProgram>& program_handle, const bul::handle<Image>& image_handle,
                    uint32_t binding);
    void bind_uniform_buffer(const bul::handle<ComputeProgram>& program_handle,
                             const bul::handle<Buffer>& buffer_handle, uint32_t binding, uint32_t offset,
                             uint32_t size);
    void bind_storage_buffer(const bul::handle<ComputeProgram>& program_handle,
                             const bul::handle<Buffer>& buffer_handle, uint32_t binding);

    void upload_buffer(const bul::handle<Buffer>& buffer_handle, void* data, uint32_t size);
    void upload_image(const bul::handle<Image>& image_handle, void* data, uint32_t size);
    void upload_image(const bul::handle<Image>& image_handle, const std::string& path);
    void blit_image(const bul::handle<Image>& src, const bul::handle<Image>& dst);

    void bind_descriptor_set(const bul::handle<ComputeProgram>& program_handle, DescriptorSet& set, uint32_t set_index);
    void bind_pipeline(const bul::handle<ComputeProgram>& program_handle);
    void dispatch(uint32_t x, uint32_t y, uint32_t z = 1);

    void set_scissor(const VkRect2D& rect);
    void set_viewport(const VkViewport& viewport);

    void bind_index_buffer(const bul::handle<Buffer>& buffer_handle, VkIndexType index_type, uint32_t offset);
    void bind_descriptor_set(const bul::handle<GraphicsProgram>& program_handle, DescriptorSet& set,
                             uint32_t set_index);
    void bind_pipeline(const bul::handle<GraphicsProgram>& program_handle, uint32_t pipeline_index = 0);

    void begin_renderpass(const bul::handle<FrameBuffer>& framebuffer_handle, const std::vector<LoadOp>& load_ops);
    void end_renderpass();

    void draw(uint32_t vertex_count, uint32_t first_vertex = 0);
    void draw_indexed(uint32_t index_count, uint32_t first_index = 0, uint32_t vertex_offset = 0);

    void begin_rendering(const bul::handle<FrameBuffer>& framebuffer_handle,
                                          const std::vector<LoadOp>& load_ops);
    void end_rendering(); */

    void begin();
    void end();

    void bind_descriptor_buffer(bul::handle<graphics_pipeline> pipeline_handle);

    void begin_rendering(bul::span<bul::handle<image>> color_attachments, bul::span<load_op> color_load_ops,
                         bul::handle<image> depth_attachment, load_op depth_load_op);
    void end_rendering();

    void push_constant(bul::handle<graphics_pipeline> handle, void* data, uint32_t size);

    void bind_index_buffer(bul::handle<buffer> index_buffer_handle,
                           uint32_t offset = 0, VkIndexType index_type = VK_INDEX_TYPE_UINT32);
    void bind_pipeline(bul::handle<graphics_pipeline> handle, graphics_state graphics_state);

    void set_scissor(const VkRect2D& rect);
    void set_viewport(const VkViewport& viewport);

    void draw(uint32_t vertex_count, uint32_t first_vertex = 0);
    void draw_indexed(uint32_t index_count, uint32_t first_index = 0, uint32_t vertex_offset = 0);

    void barrier(bul::handle<image> handle, image_usage dst_usage);

    void upload_buffer(bul::handle<buffer> buffer_handle, bul::handle<buffer> staging_buffer_handle, void* data,
                       uint32_t size, uint32_t src_offset = 0, uint32_t dst_offset = 0);
    void upload_image(bul::handle<image> image_handle, bul::handle<buffer> staging_buffer_handle, void* data,
                      uint32_t size);

    context* context;
    VkCommandBuffer vk_handle;
    VkQueue vk_queue;
    VkPipelineStageFlags wait_stage;
};

struct command_pool
{
    static command_pool create(context* context, uint32_t queue_index, VkQueue vk_queue);
    void destroy();
    void reset();
    command_buffer* get_command_buffer();

    context* context;
    VkCommandPool vk_handle;
    VkQueue vk_queue;
    bul::vector<command_buffer> command_buffers;
    uint32_t current_index = 0;
};
} // namespace vk
