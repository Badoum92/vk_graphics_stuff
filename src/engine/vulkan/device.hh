#pragma once

#include <functional>
#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "handle.hh"
#include "pool.hh"

#include "context.hh"
#include "surface.hh"
#include "image.hh"
#include "buffer.hh"
#include "framebuffer.hh"
#include "shader.hh"
#include "graphics_pipeline.hh"
#include "descriptor_set.hh"
#include "frame_context.hh"
#include "command.hh"

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

    Pool<Image> images;
    Pool<Buffer> buffers;
    Pool<FrameBuffer> framebuffers;
    Pool<Shader> shaders;
    Pool<GraphicsProgram> graphics_programs;
    std::vector<VkSampler> samplers;
    std::vector<FrameContext> frame_contexts;

    DescriptorSet global_uniform_set;

    static Device create(const Context& context);
    void destroy();

    Handle<Image> create_image(const ImageDescription& description, VkImage vk_image = VK_NULL_HANDLE);
    Handle<Image> create_image(const ImageDescription& description, const std::string& path);
    void destroy_image(const Handle<Image>& handle);

    Handle<Buffer> create_buffer(const BufferDescription& description);
    void destroy_buffer(const Handle<Buffer>& handle);
    void* map_buffer(const Handle<Buffer>& handle);
    void unmap_buffer(const Handle<Buffer>& handle);

    RenderPass create_renderpass(const FrameBufferDescription& description, const std::vector<LoadOp> load_ops);
    Handle<FrameBuffer> create_framebuffer(const FrameBufferDescription& description,
                                           const std::vector<Handle<Image>>& color_attachments,
                                           Handle<Image> depth_attachment, const std::vector<LoadOp> load_ops);
    void destroy_framebuffer(const Handle<FrameBuffer>& handle);

    DescriptorSet create_descriptor_set(const std::vector<DescriptorType>& descriptor_types);
    void destroy_descriptor_set(DescriptorSet& descriptor_set);

    Handle<Shader> create_shader(const std::string& path);
    void destroy_shader(const Handle<Shader>& handle);

    Handle<GraphicsProgram> create_graphics_program(const GraphicsProgramDescription& description);
    void destroy_graphics_program(const Handle<GraphicsProgram>& handle);
    VkPipeline compile(const Handle<GraphicsProgram>& handle, const RenderState& render_state);

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
