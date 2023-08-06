#include "device.h"

#include <vector>
#include <array>

#include "vk_tools.h"
#include "context.h"
#include "image.h"

namespace vk
{
static const std::vector<const char*> device_ext = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

void device::create()
{
    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(context::physical_device.vk_handle, &family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(context::physical_device.vk_handle, &family_count, queue_families.data());

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    float priority = 1.0;

    for (uint32_t i = 0; i < queue_families.size(); i++)
    {
        VkDeviceQueueCreateInfo queue_info{};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = i;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = &priority;

        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && device::graphics_queue.index == UINT32_MAX)
        {
            device::graphics_queue.index = i;
        }
        else if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT && device::compute_queue.index == UINT32_MAX)
        {
            device::compute_queue.index = i;
        }
        else if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT && device::transfer_queue.index == UINT32_MAX)
        {
            device::transfer_queue.index = i;
        }
        else
        {
            continue;
        }
        queue_create_infos.push_back(queue_info);
    }

    VkPhysicalDeviceFeatures2 vulkan_features{};
    vulkan_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vulkan_features.features.shaderInt64 = true;

    VkPhysicalDeviceVulkan12Features vulkan12_features{};
    vulkan12_features.pNext = &vulkan_features;
    vulkan12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
    vulkan12_features.bufferDeviceAddress = true;
    vulkan12_features.descriptorIndexing = true;
    vulkan12_features.runtimeDescriptorArray = true;
    vulkan12_features.descriptorBindingPartiallyBound = true;
    vulkan12_features.shaderSampledImageArrayNonUniformIndexing = true;
    vulkan12_features.descriptorBindingSampledImageUpdateAfterBind = true;

    VkPhysicalDeviceVulkan13Features vulkan13_features{};
    vulkan13_features.pNext = &vulkan12_features;
    vulkan13_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
    vulkan13_features.synchronization2 = true;
    vulkan13_features.dynamicRendering = true;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pNext = &vulkan13_features;
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = queue_create_infos.size();
    create_info.enabledExtensionCount = device_ext.size();
    create_info.ppEnabledExtensionNames = device_ext.data();
    create_info.enabledLayerCount = 0;

    VK_CHECK(vkCreateDevice(context::physical_device.vk_handle, &create_info, nullptr, &device::vk_handle));
    volkLoadDevice(device::vk_handle);

    const uint32_t descriptor_count = 256;
    std::array pool_sizes{
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, .descriptorCount = descriptor_count},
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, .descriptorCount = descriptor_count},
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, .descriptorCount = descriptor_count},
        VkDescriptorPoolSize{.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, .descriptorCount = descriptor_count},
    };
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.pPoolSizes = pool_sizes.data();
    pool_info.maxSets = descriptor_count;

    vkCreateDescriptorPool(device::vk_handle, &pool_info, nullptr, &device::descriptor_pool);

    VmaVulkanFunctions vma_functions = {};
    vma_functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vma_functions.vkGetDeviceProcAddr   = vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
    allocator_info.instance = context::instance;
    allocator_info.physicalDevice = context::physical_device.vk_handle;
    allocator_info.device = device::vk_handle;
    allocator_info.pVulkanFunctions = &vma_functions;
    VK_CHECK(vmaCreateAllocator(&allocator_info, &device::allocator));

    vkGetDeviceQueue(device::vk_handle, device::graphics_queue.index, 0, &device::graphics_queue.vk_handle);
    vkGetDeviceQueue(device::vk_handle, device::compute_queue.index, 0, &device::compute_queue.vk_handle);
    vkGetDeviceQueue(device::vk_handle, device::transfer_queue.index, 0, &device::transfer_queue.vk_handle);

    VkSamplerCreateInfo sampler_info{};
    sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler_info.magFilter = VK_FILTER_NEAREST;
    sampler_info.minFilter = VK_FILTER_NEAREST;
    sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    sampler_info.anisotropyEnable = VK_FALSE;
    sampler_info.maxAnisotropy = 1.0f;
    sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    sampler_info.unnormalizedCoordinates = VK_FALSE;
    sampler_info.compareEnable = VK_FALSE;
    sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
    sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler_info.mipLodBias = 0.0f;
    sampler_info.minLod = 0.0f;
    sampler_info.maxLod = 0.0f;

    device::samplers.resize(1);
    vkCreateSampler(device::vk_handle, &sampler_info, nullptr, &device::samplers.back());

    device::create_frame_contexts();

    device::global_uniform_set =
        device::create_descriptor_set({DescriptorType::create(DescriptorType::Type::DynamicBuffer)});
}

void device::destroy()
{
    wait_idle();

    destroy_descriptor_set(global_uniform_set);

    destroy_frame_contexts();

    for (auto& image : images)
    {
        image.destroy();
    }
    images.clear();
    for (auto& buffer : buffers)
    {
        buffer.destroy();
    }
    buffers.clear();
    for (auto& framebuffer : framebuffers)
    {
        framebuffer.destroy();
    }
    framebuffers.clear();
    for (auto& shader : shaders)
    {
        destroy_shader(shader);
    }
    shaders.clear();
    for (auto& graphics_program : graphics_programs)
    {
        destroy_graphics_program(graphics_program);
    }
    graphics_programs.clear();
    for (auto& compute_program : compute_programs)
    {
        destroy_compute_program(compute_program);
    }
    compute_programs.clear();
    for (auto& sampler : samplers)
    {
        vkDestroySampler(vk_handle, sampler, nullptr);
    }

    vkDestroyDescriptorPool(vk_handle, descriptor_pool, nullptr);
    vmaDestroyAllocator(allocator);
    allocator = VK_NULL_HANDLE;
    vkDestroyDevice(vk_handle, nullptr);
    vk_handle = VK_NULL_HANDLE;
}

void device::wait_idle()
{
    VK_CHECK(vkDeviceWaitIdle(vk_handle));
}

bool device::acquire_next_image()
{
    auto& fc = frame_contexts[current_frame];
    vkWaitForFences(vk_handle, 1, &fc.rendering_finished_fence, VK_TRUE, UINT64_MAX);
    auto res = vkAcquireNextImageKHR(vk_handle, surface::swapchain, UINT64_MAX, fc.image_acquired_semaphore,
                                     VK_NULL_HANDLE, &fc.image_index);

    fc.command_context.reset();
    fc.framebuffer = surface::framebuffers[fc.image_index];
    fc.image = surface::images[fc.image_index];

    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return false;
    }

    VK_CHECK(res);

    return true;
}

bool device::present()
{
    auto& fc = frame_contexts[current_frame];
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &fc.rendering_finished_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &surface::swapchain;
    present_info.pImageIndices = &fc.image_index;

    auto res = vkQueuePresentKHR(graphics_queue.vk_handle, &present_info);

    current_frame = (current_frame + 1) % MAX_FRAMES;

    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return false;
    }

    VK_CHECK(res);

    return true;
}
} // namespace vk
