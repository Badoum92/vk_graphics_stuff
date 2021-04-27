#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-extension"
#include <vma/vk_mem_alloc.h>
#pragma clang diagnostic pop

#include <vulkan/vulkan.h>
#include <vector>

#include "instance/instance.hh"
#include "surface/surface.hh"
#include "physical_device/physical_device.hh"
#include "device/device.hh"
#include "swapchain/swapchain.hh"
#include "framebuffer/framebuffer.hh"
#include "render_pass/render_pass.hh"
#include "pipeline/pipeline.hh"
#include "command_pool/command_pool.hh"
#include "command_buffers/command_buffers.hh"
#include "buffer/buffer.hh"

#include "vk_tools.hh"

struct VkContext
{
    static const int FRAMES_IN_FLIGHT;
    static size_t current_frame;

    static void draw_frame();
    static void wait_idle();

    static void create();
    static void destroy();

    static void refresh();

    static Instance instance;
    static Surface surface;
    static PhysicalDevice physical_device;
    static Device device;
    static CommandPool command_pool;
    static SwapChain swapchain;
    static RenderPass renderpass;
    static Pipeline pipeline;
    static std::vector<FrameBuffer> framebuffers;
    static CommandBuffers command_buffers;

    static std::vector<VkSemaphore> image_available_semaphores;
    static std::vector<VkSemaphore> render_finished_semaphores;
    static std::vector<VkFence> in_flight_fences;
    static std::vector<VkFence> images_in_flight;

    static VmaAllocator allocator;

    /* --- */

    static Buffer vertex_buffer;
    static Buffer index_buffer;

private:
    static void inner_create();
    static void inner_destroy();

    static void create_sync();
    static void destroy_sync();
};
