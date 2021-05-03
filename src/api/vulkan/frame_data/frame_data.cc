#include "frame_data.hh"

#include "vk_context/vk_context.hh"

void FrameData::create()
{
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VK_CHECK(vkCreateSemaphore(VkContext::device, &semaphore_info, nullptr, &present_semaphore));
    VK_CHECK(vkCreateSemaphore(VkContext::device, &semaphore_info, nullptr, &render_semaphore));
    VK_CHECK(vkCreateFence(VkContext::device, &fence_info, nullptr, &render_fence));

    command_pool.create();
    cmd_buffer = command_pool.allocate_command_buffer();
}

void FrameData::destroy()
{
    vkDestroySemaphore(VkContext::device, present_semaphore, nullptr);
    vkDestroySemaphore(VkContext::device, render_semaphore, nullptr);
    vkDestroyFence(VkContext::device, render_fence, nullptr);

    cmd_buffer.destroy();
    command_pool.destroy();
}

void FrameData::begin_frame()
{
    command_pool.reset();
    cmd_buffer.begin();
}

void FrameData::end_frame()
{
    cmd_buffer.end();
}

FrameData& FrameData::current()
{
    return VkContext::frames[VkContext::current_frame];
}
