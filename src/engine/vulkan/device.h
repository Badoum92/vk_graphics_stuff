#pragma once

#include <functional>
#include <volk.h>
#include <vma/vk_mem_alloc.h>

#include "bul/containers/handle.h"
#include "bul/containers/pool.h"

#include "context.h"
#include "surface.h"
#include "image.h"
#include "buffer.h"
#include "framebuffer.h"
#include "shader.h"
#include "graphics_pipeline.h"
#include "compute_pipeline.h"
#include "descriptor_set.h"
#include "frame_context.h"
#include "command.h"

namespace vk
{
struct Device
{
    static constexpr uint32_t MAX_FRAMES = 2;
    uint32_t current_frame = 0;

    PhysicalDevice physical_device;
    VkDevice vk_handle = VK_NULL_HANDLE;
    uint32_t graphics_family_index = UINT32_MAX;
    uint32_t compute_family_index = UINT32_MAX;
    uint32_t transfer_family_index = UINT32_MAX;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkQueue compute_queue = VK_NULL_HANDLE;
    VkQueue transfer_queue = VK_NULL_HANDLE;
    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    VmaAllocator allocator = VK_NULL_HANDLE;

    bul::Pool<Image> images;
    bul::Pool<Buffer> buffers;
    bul::Pool<FrameBuffer> framebuffers;
    bul::Pool<Shader> shaders;
    bul::Pool<GraphicsProgram> graphics_programs;
    bul::Pool<ComputeProgram> compute_programs;
    std::vector<VkSampler> samplers;
    FrameContext frame_contexts[MAX_FRAMES];

    DescriptorSet global_uniform_set;

    static Device create(const Context& context);
    void destroy();

    bul::Handle<Image> create_image(const ImageDescription& description, VkImage vk_image = VK_NULL_HANDLE);
    bul::Handle<Image> create_image(const ImageDescription& description, const std::string& path);
    void destroy_image(Image& image);

    bul::Handle<Buffer> create_buffer(const BufferDescription& description);
    void destroy_buffer(Buffer& buffer);
    void* map_buffer(Buffer& buffer);
    void unmap_buffer(Buffer& buffer);

    RenderPass create_renderpass(const FrameBufferDescription& description, const std::vector<LoadOp>& load_ops);
    const RenderPass& get_or_create_renderpass(const bul::Handle<FrameBuffer>& handle, const std::vector<LoadOp>& load_ops);
    bul::Handle<FrameBuffer> create_framebuffer(const FrameBufferDescription& description,
                                           const std::vector<bul::Handle<Image>>& color_attachments,
                                           const bul::Handle<Image>& depth_attachment);
    void destroy_framebuffer(FrameBuffer& framebuffer);

    DescriptorSet create_descriptor_set(const std::vector<DescriptorType>& descriptor_types);
    void destroy_descriptor_set(DescriptorSet& descriptor_set);

    bul::Handle<Shader> create_shader(const std::string& path);
    void destroy_shader(Shader& shader);

    bul::Handle<GraphicsProgram> create_graphics_program(const GraphicsProgramDescription& description);
    void destroy_graphics_program(GraphicsProgram& graphics_program);
    VkPipeline compile(const bul::Handle<GraphicsProgram>& handle, const RenderState& render_state);

    bul::Handle<ComputeProgram> create_compute_program(const ComputeProgramDescription& description);
    void destroy_compute_program(ComputeProgram& compute_program);

    void create_frame_contexts();
    void destroy_frame_contexts();
    FrameContext& frame_context();

    GraphicsCommand& get_graphics_command();
    ComputeCommand& get_compute_command();
    TransferCommand& get_transfer_command();
    void submit(Command& command, VkSemaphore wait_semaphore, VkSemaphore signal_semaphore, VkFence fence);
    void submit(Command& command);
    void submit_blocking(Command& command);

    void wait_idle();
    bool acquire_next_image(Surface& surface);
    bool present(Surface& surface);
};
} // namespace vk
