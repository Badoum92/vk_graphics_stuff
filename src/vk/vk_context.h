#pragma once

#include <volk.h>
#include <vma/vk_mem_alloc.h>

#include "vk/vk_constants.h"
#include "vk/vk_surface.h"
#include "vk/vk_descriptor.h"
#include "vk/vk_commands.h"

#include "core/containers/pool.h"

struct window;

namespace vk
{
struct image_description;
struct image;
struct sampler_description;
struct sampler;
struct buffer_description;
struct buffer;
struct shader;
struct graphics_pipeline_description;
struct graphics_pipeline;
struct compute_pipeline_description;
struct compute_pipeline;

struct frame_context
{
    VkSemaphore image_acquired_semaphore = VK_NULL_HANDLE;
    VkSemaphore rendering_finished_semaphore = VK_NULL_HANDLE;
    VkFence rendering_finished_fence = VK_NULL_HANDLE;
    command_pool graphics_commands;
    command_pool compute_commands;
    command_buffer* command_buffer;
    uint32_t image_index;
    image* image;
};

struct context
{
    static void create(context* context, window* _window, bool enable_validation);
    void destroy();

    void wait_idle();
    frame_context* get_frame_context();
    frame_context* acquire_next_image();
    bool present(frame_context* frame_context);
    void submit(command_buffer* command_buffer);
    void submit(command_buffer* command_buffer, frame_context* frame_context);

    void set_vsync(bool vsync);

    frame_context frame_contexts[max_frames_in_flight];
    uint32_t current_frame = 0;
    bool vsync = false;

    window* window;

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

    descriptor_set texture_descriptor_set;
    descriptor_set image_descriptor_set;

    command_pool transfer_commands;

    VmaAllocator vma_allocator = VK_NULL_HANDLE;

    sampler* default_sampler;
    image* undefined_image;
    uint32_t undefined_descriptor = UINT32_MAX;

    pool<image> images;
    pool<sampler> samplers;
    pool<buffer> buffers;
    pool<shader> shaders;
    pool<graphics_pipeline> graphics_pipelines;
    pool<compute_pipeline> compute_pipelines;

    VkDescriptorPool descriptor_pool; // only used for ImGui

    image* create_image(const image_description& description, VkImage vk_image = VK_NULL_HANDLE);
    void destroy_image(image* image);

    sampler* create_sampler(const sampler_description& description);
    void destroy_sampler(sampler* sampler);

    buffer* create_buffer(const buffer_description& description);
    void destroy_buffer(buffer* buffer);

    shader* create_shader(const char* path);
    void destroy_shader(shader* shader);

    graphics_pipeline* create_graphics_pipeline(const graphics_pipeline_description& description);
    VkPipeline compile_graphics_pipeline(graphics_pipeline* pipeline, graphics_state graphics_state);
    void destroy_graphics_pipeline(graphics_pipeline* pipeline);

    compute_pipeline* create_compute_pipeline(const compute_pipeline_description& description);
    void destroy_compute_pipeline(compute_pipeline* pipeline);
};
} // namespace vk
