#include "vk_context.hh"

#include <vma/vk_mem_alloc.h>
#include <stdexcept>
#include <iostream>

#include "window/window.hh"

size_t VkContext::current_frame = 0;
std::array<FrameData, VkContext::FRAMES_IN_FLIGHT> VkContext::frames;
uint32_t VkContext::image_index;

Instance VkContext::instance;
Surface VkContext::surface;
PhysicalDevice VkContext::physical_device;
Device VkContext::device;
CommandPool VkContext::command_pool;
DescriptorPool VkContext::descriptor_pool;
SwapChain VkContext::swapchain;
RenderPass VkContext::renderpass;
std::vector<FrameBuffer> VkContext::framebuffers;

VmaAllocator VkContext::allocator;

void VkContext::create()
{
    instance.create();
    surface.create();
    physical_device.create();
    device.create();
    command_pool.create();
    descriptor_pool.create();

    VmaAllocatorCreateInfo allocator_info = {};
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
    allocator_info.instance = instance;
    allocator_info.physicalDevice = physical_device;
    allocator_info.device = device;
    vmaCreateAllocator(&allocator_info, &allocator);

    renderpass.create();

    inner_create();
}

void VkContext::destroy()
{
    inner_destroy();

    DescriptorSet::global_set.destroy();
    renderpass.destroy();

    vmaDestroyAllocator(allocator);

    descriptor_pool.destroy();
    command_pool.destroy();
    device.destroy();
    physical_device.destroy();
    surface.destroy();
    instance.destroy();
}

void VkContext::inner_create()
{
    swapchain.create();

    framebuffers.resize(swapchain.size());
    for (size_t i = 0; i < framebuffers.size(); ++i)
    {
        framebuffers[i].create(renderpass, swapchain.image_views()[i]);
    }

    for (size_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
    {
        frames[i].create();
    }
}

void VkContext::inner_destroy()
{
    for (size_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
    {
        frames[i].destroy();
    }

    for (auto& framebuffer : framebuffers)
    {
        framebuffer.destroy();
    }

    swapchain.destroy();
}

void VkContext::refresh()
{
    while (Window::width() == 0 || Window::height() == 0)
    {
        Window::wait_events();
        Window::resized();
    }

    wait_idle();

    inner_destroy();
    inner_create();
}

void VkContext::wait_idle()
{
    vkDeviceWaitIdle(device);
}

std::optional<std::pair<CommandBuffer, FrameBuffer>> VkContext::begin_frame()
{
    vkWaitForFences(device, 1, &frames[current_frame].render_fence, VK_TRUE, UINT64_MAX);

    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, frames[current_frame].render_semaphore,
                                            VK_NULL_HANDLE, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        refresh();
        return std::nullopt;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swapchain image");
    }

    // if (images_in_flight[image_index] != VK_NULL_HANDLE)
    // {
    //     vkWaitForFences(device, 1, &images_in_flight[image_index], VK_TRUE, UINT64_MAX);
    // }
    // images_in_flight[image_index] = in_flight_fences[current_frame];

    frames[current_frame].begin_frame();
    return std::make_pair(frames[current_frame].cmd_buffer, framebuffers[image_index]);
}

void VkContext::end_frame()
{
    frames[current_frame].end_frame();

    VkSemaphore wait_semaphores[] = {frames[current_frame].render_semaphore};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &frames[current_frame].cmd_buffer.handle();

    VkSemaphore signal_semaphores[] = {frames[current_frame].present_semaphore};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(device, 1, &frames[current_frame].render_fence);

    VK_CHECK(vkQueueSubmit(device.graphics_queue(), 1, &submit_info, frames[current_frame].render_fence));

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    VkSwapchainKHR swapchains[] = {swapchain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    VkResult result = vkQueuePresentKHR(device.present_queue(), &present_info);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || Window::resized())
    {
        refresh();
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to present swapchain image");
    }

    current_frame = (current_frame + 1) % FRAMES_IN_FLIGHT;
}
