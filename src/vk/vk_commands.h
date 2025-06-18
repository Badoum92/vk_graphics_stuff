#pragma once

#include <volk.h>

#include "core/containers/vector.h"

namespace vk
{
struct context;
struct image;
struct buffer;
struct graphics_pipeline;
struct compute_pipeline;
struct load_op;
struct graphics_state;
enum class image_usage;

struct command_buffer
{
    void begin();
    void end();

    void bind_descriptor_buffer(graphics_pipeline* pipeline);
    void bind_descriptor_buffer(compute_pipeline* pipeline);

    void begin_rendering(bul::span<image*> color_attachments, bul::span<load_op> color_load_ops,
                         image* depth_attachment, load_op depth_load_op);
    void end_rendering();

    void push_constant(graphics_pipeline* pipeline, void* data, uint32_t size);
    void push_constant(compute_pipeline* pipeline, void* data, uint32_t size);

    void bind_index_buffer(buffer* buffer, uint32_t offset = 0, VkIndexType index_type = VK_INDEX_TYPE_UINT32);
    void bind_graphics_pipeline(graphics_pipeline* pipeline, const graphics_state& graphics_state);
    void bind_compute_pipeline(compute_pipeline* pipeline);

    void set_scissor(const VkRect2D& rect);
    void set_viewport(const VkViewport& viewport);

    void draw(uint32_t vertex_count, uint32_t first_vertex = 0);
    void draw_indexed(uint32_t index_count, uint32_t first_index = 0, uint32_t vertex_offset = 0);
    void compute_dispatch(uint32_t x, uint32_t y, uint32_t z);

    void barrier(image* image, const image_usage& dst_usage);

    void copy_buffer_to_buffer(buffer* src, buffer* dst, uint32_t size = 0, uint32_t src_offset = 0,
                               uint32_t dst_offset = 0);
    void copy_image_to_buffer(image* image, buffer* buffer, uint32_t dst_offset = 0);
    void copy_buffer_to_image(buffer* buffer, image* image, uint32_t src_offset = 0);

    void upload_buffer(buffer* buffer, ::vk::buffer* staging_buffer, void* data, uint32_t size, uint32_t src_offset = 0,
                       uint32_t dst_offset = 0);
    void upload_image(image* image, buffer* staging, void* data, uint32_t size);

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
