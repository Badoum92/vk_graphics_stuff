#pragma once

#include <volk.h>
#include <vma/vk_mem_alloc.h>

#include "vk/vk_tools.h"
#include "vk/surface.h"
#include "vk/image.h"
#include "vk/buffer.h"
#include "vk/shader.h"

#include "bul/containers/pool.h"

namespace vk
{
struct context
{
    static context create(bool enable_validation);
    void destroy();

    static constexpr uint32_t MAX_FRAMES = 2;

    VkInstance instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debug_messenger = VK_NULL_HANDLE;

    VkPhysicalDevice physical_device = VK_NULL_HANDLE;
    VkPhysicalDeviceProperties physical_device_properties;

    VkDevice device = VK_NULL_HANDLE;
    uint32_t graphics_queue_index = UINT32_MAX;
    uint32_t compute_queue_index = UINT32_MAX;
    uint32_t transfer_queue_index = UINT32_MAX;
    VkQueue graphics_queue = VK_NULL_HANDLE;
    VkQueue compute_queue = VK_NULL_HANDLE;
    VkQueue transfer_queue = VK_NULL_HANDLE;

    VmaAllocator allocator = VK_NULL_HANDLE;

    VkDescriptorPool descriptor_pool = VK_NULL_HANDLE;

    surface surface;

    bul::pool<image> images;
    bul::pool<buffer> buffers;
    bul::pool<shader> shaders;

    bul::handle<image> create_image(image_description&& description, VkImage vk_image = VK_NULL_HANDLE);
    void destroy_image(bul::handle<image> handle);

    bul::handle<buffer> create_buffer(buffer_description&& description);
    void destroy_buffer(bul::handle<buffer> handle);

    bul::handle<shader> create_shader(const std::string_view path);
    void destroy_shader(bul::handle<shader> handle);
};
} // namespace vk
