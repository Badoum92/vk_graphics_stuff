#include "device.hh"

#include <stdexcept>
#include <unordered_set>

#include "vk_context/vk_context.hh"

void Device::create()
{
    auto indices = VkContext::physical_device.get_queue_family_indices();

    float queue_priority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::unordered_set<uint32_t> queue_families = {indices.graphics_family.value(), indices.present_family.value()};

    for (uint32_t queue_family : queue_families)
    {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priority;
        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = queue_create_infos.size();
    create_info.pEnabledFeatures = &device_features;
    create_info.enabledExtensionCount = PhysicalDevice::device_ext.size();
    create_info.ppEnabledExtensionNames = PhysicalDevice::device_ext.data();
    create_info.enabledLayerCount = 0;

    VK_CHECK(vkCreateDevice(VkContext::physical_device, &create_info, nullptr, &handle_));

    vkGetDeviceQueue(handle_, indices.graphics_family.value(), 0, &graphics_queue_);
    vkGetDeviceQueue(handle_, indices.present_family.value(), 0, &present_queue_);
}

void Device::destroy()
{
    vkDestroyDevice(handle_, nullptr);
}

VkQueue Device::graphics_queue() const
{
    return graphics_queue_;
}

VkQueue Device::present_queue() const
{
    return present_queue_;
}
