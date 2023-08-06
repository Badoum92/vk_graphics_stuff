#include "frame_context.h"

#include "vk_tools.h"
#include "device.h"

namespace vk
{
void device::create_frame_contexts()
{
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkCommandPoolCreateInfo pool_info{};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = 0;

    for (auto& fc : frame_contexts)
    {
        VK_CHECK(vkCreateSemaphore(vk_handle, &semaphore_info, nullptr, &fc.image_acquired_semaphore));
        VK_CHECK(vkCreateSemaphore(vk_handle, &semaphore_info, nullptr, &fc.rendering_finished_semaphore));
        VK_CHECK(vkCreateFence(vk_handle, &fence_info, nullptr, &fc.rendering_finished_fence));
        fc.command_context = CommandContext::create();
    }
}

void device::destroy_frame_contexts()
{
    for (auto& fc : frame_contexts)
    {
        vkDestroySemaphore(vk_handle, fc.image_acquired_semaphore, nullptr);
        vkDestroySemaphore(vk_handle, fc.rendering_finished_semaphore, nullptr);
        vkDestroyFence(vk_handle, fc.rendering_finished_fence, nullptr);
        fc.command_context.destroy();
    }
}

FrameContext& device::frame_context()
{
    return frame_contexts[current_frame];
}
} // namespace vk
