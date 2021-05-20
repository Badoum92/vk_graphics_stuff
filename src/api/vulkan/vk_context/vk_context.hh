#pragma once

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
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
#include "descriptor_pool/descriptor_pool.hh"
#include "frame_data/frame_data.hh"
#include "image/image.hh"

#include "vk_tools.hh"

struct VkContext
{
    static constexpr int FRAMES_IN_FLIGHT{2};
    static size_t current_frame;
    static std::array<FrameData, FRAMES_IN_FLIGHT> frames;
    static uint32_t image_index;

    static void wait_idle();

    static void create();
    static void destroy();

    static void refresh();

    static std::optional<std::pair<CommandBuffer, FrameBuffer>> begin_frame();
    static void end_frame();

    static Instance instance;
    static Surface surface;
    static PhysicalDevice physical_device;
    static Device device;
    static CommandPool command_pool;
    static DescriptorPool descriptor_pool;
    static SwapChain swapchain;
    static RenderPass renderpass;
    static std::vector<FrameBuffer> framebuffers;
    static Image depth_buffer;

    static VmaAllocator allocator;

private:
    static void inner_create();
    static void inner_destroy();
};

struct GlobalUniform
{
    glm::mat4 view;
    glm::mat4 inv_view;
    glm::mat4 proj;
    glm::mat4 inv_proj;
    glm::vec3 camera_pos;
    float pad0;
    glm::ivec2 resolution;
    glm::vec2 pad1;
};
