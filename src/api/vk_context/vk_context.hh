#pragma once

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

#include "instance/instance.hh"
#include "surface/surface.hh"
#include "physical_device/physical_device.hh"
#include "device/device.hh"
#include "swapchain/swapchain.hh"
#include "framebuffer/framebuffer.hh"
#include "render_pass/render_pass.hh"
#include "pipeline/pipeline.hh"
#include "command_pool/command_pool.hh"
#include "buffers/buffer.hh"
#include "descriptor_pool/descriptor_pool.hh"
#include "frame_data/frame_data.hh"

#include "vk_tools.hh"

struct VkContext
{
    static constexpr int FRAMES_IN_FLIGHT{2};
    static size_t current_frame;
    static std::array<FrameData, FRAMES_IN_FLIGHT> frames;

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
    static DescriptorPool descriptor_pool;
    static SwapChain swapchain;
    static RenderPass renderpass;
    static Pipeline pipeline;
    static std::vector<FrameBuffer> framebuffers;

    static VmaAllocator allocator;

    /* --- */

    static Buffer vertex_buffer;
    static Buffer index_buffer;
    static Buffer global_uniform_buffer;

private:
    static void inner_create();
    static void inner_destroy();
};
