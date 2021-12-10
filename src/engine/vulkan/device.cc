#include "device.hh"

#include <vector>
#include <array>

#include "vk_tools.hh"
#include "context.hh"
#include "image.hh"

namespace vk
{
static const std::vector<const char*> device_ext = {VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                                                    VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                                                    VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME};

Device Device::create(const Context& context)
{
    Device device{};
    device.physical_device = context.physical_device;

    uint32_t family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device.physical_device.vk_handle, &family_count, nullptr);
    std::vector<VkQueueFamilyProperties> queue_families(family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(device.physical_device.vk_handle, &family_count, queue_families.data());

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    float priority = 1.0;

    for (uint32_t i = 0; i < queue_families.size(); i++)
    {
        VkDeviceQueueCreateInfo queue_info{};
        queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_info.queueFamilyIndex = i;
        queue_info.queueCount = 1;
        queue_info.pQueuePriorities = &priority;

        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && device.graphics_family_index == UINT32_MAX)
        {
            device.graphics_family_index = i;
        }
        else if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT && device.compute_family_index == UINT32_MAX)
        {
            device.compute_family_index = i;
        }
        else if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT && device.transfer_family_index == UINT32_MAX)
        {
            device.transfer_family_index = i;
        }
        else
        {
            continue;
        }
        queue_create_infos.push_back(queue_info);
    }

    VkPhysicalDeviceFeatures device_features{};
    device_features.fillModeNonSolid = true;
    device_features.wideLines = true;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = queue_create_infos.size();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = device_ext.size();
    create_info.ppEnabledExtensionNames = device_ext.data();
    create_info.enabledLayerCount = 0;

    VK_CHECK(vkCreateDevice(context.physical_device.vk_handle, &create_info, nullptr, &device.vk_handle));

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

    vkCreateDescriptorPool(device.vk_handle, &pool_info, nullptr, &device.descriptor_pool);

    VmaAllocatorCreateInfo allocator_info{};
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
    allocator_info.instance = context.instance;
    allocator_info.physicalDevice = device.physical_device.vk_handle;
    allocator_info.device = device.vk_handle;
    VK_CHECK(vmaCreateAllocator(&allocator_info, &device.allocator));

    vkGetDeviceQueue(device.vk_handle, device.graphics_family_index, 0, &device.graphics_queue);
    vkGetDeviceQueue(device.vk_handle, device.compute_family_index, 0, &device.compute_queue);
    vkGetDeviceQueue(device.vk_handle, device.transfer_family_index, 0, &device.transfer_queue);

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

    device.samplers.resize(1);
    vkCreateSampler(device.vk_handle, &sampler_info, nullptr, &device.samplers.back());

    device.create_frame_contexts();

    device.global_uniform_set =
        device.create_descriptor_set({DescriptorType::create(DescriptorType::Type::DynamicBuffer)});

    return device;
}

void Device::destroy()
{
    wait_idle();

    destroy_descriptor_set(global_uniform_set);

    destroy_frame_contexts();

    for (auto& handle : images.handles())
    {
        destroy_image(handle);
    }
    for (auto& handle : buffers.handles())
    {
        destroy_buffer(handle);
    }
    for (auto& handle : framebuffers.handles())
    {
        destroy_framebuffer(handle);
    }
    for (auto& handle : shaders.handles())
    {
        destroy_shader(handle);
    }
    for (auto& handle : graphics_programs.handles())
    {
        destroy_graphics_program(handle);
    }
    for (auto& handle : compute_programs.handles())
    {
        destroy_compute_program(handle);
    }
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

void Device::wait_idle()
{
    VK_CHECK(vkDeviceWaitIdle(vk_handle));
}

bool Device::acquire_next_image(Surface& surface)
{
    auto& fc = frame_contexts[current_frame];
    vkWaitForFences(vk_handle, 1, &fc.rendering_finished_fence, VK_TRUE, UINT64_MAX);
    auto res = vkAcquireNextImageKHR(vk_handle, surface.swapchain, UINT64_MAX, fc.image_acquired_semaphore,
                                     VK_NULL_HANDLE, &fc.image_index);

    fc.command_context.reset();
    fc.framebuffer = surface.framebuffers[fc.image_index];
    fc.image = surface.images[fc.image_index];

    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return false;
    }

    VK_CHECK(res);

    return true;
}

bool Device::present(Surface& surface)
{
    auto& fc = frame_contexts[current_frame];
    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &fc.rendering_finished_semaphore;
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &surface.swapchain;
    present_info.pImageIndices = &fc.image_index;

    auto res = vkQueuePresentKHR(graphics_queue, &present_info);

    current_frame = (current_frame + 1) % MAX_FRAMES;

    if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return false;
    }

    VK_CHECK(res);

    return true;
}
} // namespace vk
