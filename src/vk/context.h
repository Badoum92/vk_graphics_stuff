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
struct Context
{
    static Context create(bool enable_validation);
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

    Surface surface;

    bul::Pool<Image> images;
    bul::Pool<Buffer> buffers;
    bul::Pool<Shader> shaders;

    bul::Handle<Image> create_image(ImageDescription&& description, VkImage vk_image = VK_NULL_HANDLE);
    void destroy_image(bul::Handle<Image> handle);

    bul::Handle<Buffer> create_buffer(BufferDescription&& description);
    void destroy_buffer(bul::Handle<Buffer> handle);

    bul::Handle<Shader> create_shader(const std::string_view path);
    void destroy_shader(bul::Handle<Shader> handle);
};
} // namespace vk
