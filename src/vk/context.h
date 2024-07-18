#pragma once

#include <volk.h>
#include <vma/vk_mem_alloc.h>

#include "vk/constants.h"
#include "vk/vk_tools.h"
#include "vk/surface.h"
#include "vk/image.h"
#include "vk/buffer.h"
#include "vk/shader.h"
#include "vk/pipeline.h"
#include "vk/descriptor.h"
#include "vk/commands.h"

#include "bul/window.h"
#include "bul/containers/pool.h"

namespace vk
{
struct frame_context
{
    VkSemaphore image_acquired_semaphore = VK_NULL_HANDLE;
    VkSemaphore rendering_finished_semaphore = VK_NULL_HANDLE;
    VkFence rendering_finished_fence = VK_NULL_HANDLE;
    command_pool graphics_commands;
    command_pool compute_commands;
    uint32_t image_index = 0;
    bul::handle<image> image;
};

struct context
{
    static context create(bul::window* _window, bool enable_validation);
    void destroy();

    void wait_idle();
    frame_context* get_frame_context();
    frame_context* acquire_next_image();
    bool present(frame_context* frame_context);
    void submit(command_buffer* command_buffer);
    void submit(command_buffer* command_buffer, frame_context* frame_context);

    frame_context frame_contexts[max_frames_in_flight];
    uint32_t current_frame = 0;

    bul::window* window;

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties physical_device_properties;
    VkPhysicalDeviceDescriptorBufferPropertiesEXT descriptor_buffer_properties;

    VkDevice device = VK_NULL_HANDLE;
    uint32_t graphics_queue_index = UINT32_MAX;
    uint32_t compute_queue_index = UINT32_MAX;
    uint32_t transfer_queue_index = UINT32_MAX;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkQueue compute_queue = VK_NULL_HANDLE;
    VkQueue transfer_queue = VK_NULL_HANDLE;

    surface surface;

    descriptor_set descriptor_set;

    command_pool transfer_commands;

    VmaAllocator allocator = VK_NULL_HANDLE;

    bul::handle<sampler> default_sampler;
    bul::handle<image> undefined_image_handle;
    uint32_t undefined_descriptor = UINT32_MAX;

    bul::pool<image> images;
    bul::pool<sampler> samplers;
    bul::pool<buffer> buffers;
    bul::pool<shader> shaders;
    bul::pool<graphics_pipeline> graphics_pipelines;

    bul::handle<image> create_image(const image_description& description, VkImage vk_image = VK_NULL_HANDLE);
    void destroy_image(bul::handle<image> handle);

    bul::handle<sampler> create_sampler(const sampler_description& description);
    void destroy_sampler(bul::handle<sampler> handle);

    bul::handle<buffer> create_buffer(const buffer_description& description);
    void destroy_buffer(bul::handle<buffer> handle);

    bul::handle<shader> create_shader(const char* path);
    void destroy_shader(bul::handle<shader> handle);

    bul::handle<graphics_pipeline> create_graphics_pipeline(const graphics_pipeline_description& description);
    VkPipeline compile_graphics_pipeline(bul::handle<graphics_pipeline> handle, graphics_state graphics_state);
    void destroy_graphics_pipeline(bul::handle<graphics_pipeline> handle);
};
} // namespace vk
