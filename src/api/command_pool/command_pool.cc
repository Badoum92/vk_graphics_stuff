#include "command_pool.hh"

#include "vk_context/vk_context.hh"

void CommandPool::create()
{
    QueueFamilyIndices indices = VkContext::physical_device.get_queue_family_indices();

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = indices.graphics_family.value();
    pool_info.flags = 0;

    VK_CHECK(vkCreateCommandPool(VkContext::device, &pool_info, nullptr, &handle_));
}

void CommandPool::destroy()
{
    vkDestroyCommandPool(VkContext::device, handle_, nullptr);
}
