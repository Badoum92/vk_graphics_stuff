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
namespace device
{
    struct Queue
    {
        VkQueue vk_handle = VK_NULL_HANDLE;
        uint32_t index = UINT32_MAX;
    };

    constexpr uint32_t MAX_FRAMES = 2;
    inline uint32_t current_frame = 0;
    inline FrameContext frame_contexts[MAX_FRAMES];

    inline VkDevice vk_handle = VK_NULL_HANDLE;
    inline Queue graphics_queue;
    inline Queue compute_queue;
    inline Queue transfer_queue;
    inline VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;
    inline VmaAllocator allocator = VK_NULL_HANDLE;
    inline DescriptorSet global_uniform_set;
    inline std::vector<VkSampler> samplers;

    inline bul::Pool<Image> images;
    inline bul::Pool<Buffer> buffers;
    inline bul::Pool<FrameBuffer> framebuffers;
    inline bul::Pool<Shader> shaders;
    inline bul::Pool<GraphicsProgram> graphics_programs;
    inline bul::Pool<ComputeProgram> compute_programs;

    void create();
    void destroy();

    DescriptorSet create_descriptor_set(const std::vector<DescriptorType>& descriptor_types);
    void destroy_descriptor_set(DescriptorSet& descriptor_set);

    bul::handle<Shader> create_shader(const std::string& path);
    void destroy_shader(Shader& shader);

    bul::handle<GraphicsProgram> create_graphics_program(const GraphicsProgramDescription& description);
    void destroy_graphics_program(GraphicsProgram& graphics_program);
    VkPipeline compile(const bul::handle<GraphicsProgram>& handle, const RenderState& render_state);

    bul::handle<ComputeProgram> create_compute_program(const ComputeProgramDescription& description);
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
    bool acquire_next_image();
    bool present();
};
} // namespace vk
