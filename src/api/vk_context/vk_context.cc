#include "vk_context.hh"

#include "window/window.hh"

const int VkContext::FRAMES_IN_FLIGHT = 2;
size_t VkContext::current_frame = 0;

Instance VkContext::instance;
Surface VkContext::surface;
PhysicalDevice VkContext::physical_device;
Device VkContext::device;
SwapChain VkContext::swapchain;
RenderPass VkContext::renderpass;
Pipeline VkContext::pipeline;
std::vector<FrameBuffer> VkContext::framebuffers;
CommandPool VkContext::command_pool;
CommandBuffers VkContext::command_buffers;

std::vector<VkSemaphore> VkContext::image_available_semaphores;
std::vector<VkSemaphore> VkContext::render_finished_semaphores;
std::vector<VkFence> VkContext::in_flight_fences;
std::vector<VkFence> VkContext::images_in_flight;

void VkContext::create()
{
    instance.create();
    surface.create();
    physical_device.create();
    device.create();
    command_pool.create();
    swapchain.create();
    renderpass.create();
    pipeline.create();

    framebuffers.resize(swapchain.image_views().size());
    for (size_t i = 0; i < framebuffers.size(); ++i)
    {
        framebuffers[i].create(renderpass, swapchain.image_views()[i]);
    }

    command_buffers.create();
    for (size_t i = 0; i < command_buffers.size(); ++i)
    {
        command_buffers.record(i);
    }

    create_sync();
}

void VkContext::destroy()
{
    destroy_sync();

    command_buffers.destroy();
    command_pool.destroy();

    for (auto& framebuffer : framebuffers)
    {
        framebuffer.destroy();
    }

    pipeline.destroy();
    renderpass.destroy();
    swapchain.destroy();
    device.destroy();
    physical_device.destroy();
    surface.destroy();
    instance.destroy();
}

void VkContext::refresh()
{
    while (Window::width() == 0 || Window::height() == 0)
    {
        Window::wait_events();
        Window::resized();
    }

    wait_idle();

    for (auto& framebuffer : framebuffers)
    {
        framebuffer.destroy();
    }

    command_buffers.destroy();
    pipeline.destroy();
    renderpass.destroy();
    swapchain.destroy();

    swapchain.create();
    renderpass.create();
    pipeline.create();
    for (size_t i = 0; i < framebuffers.size(); ++i)
    {
        framebuffers[i].create(renderpass, swapchain.image_views()[i]);
    }
    command_buffers.create();
    for (size_t i = 0; i < command_buffers.size(); ++i)
    {
        command_buffers.record(i);
    }
}

void VkContext::create_sync()
{
    image_available_semaphores.resize(FRAMES_IN_FLIGHT);
    render_finished_semaphores.resize(FRAMES_IN_FLIGHT);
    in_flight_fences.resize(FRAMES_IN_FLIGHT);
    images_in_flight.resize(swapchain.image_views().size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &image_available_semaphores[i]));
        VK_CHECK(vkCreateSemaphore(device, &semaphore_info, nullptr, &render_finished_semaphores[i]));
        VK_CHECK(vkCreateFence(device, &fence_info, nullptr, &in_flight_fences[i]));
    }
}

void VkContext::destroy_sync()
{
    for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(device, image_available_semaphores[i], nullptr);
        vkDestroySemaphore(device, render_finished_semaphores[i], nullptr);
        vkDestroyFence(device, in_flight_fences[i], nullptr);
    }
}

void VkContext::draw_frame()
{
    vkWaitForFences(device, 1, &in_flight_fences[current_frame], VK_TRUE, UINT64_MAX);

    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, image_available_semaphores[current_frame],
                                            VK_NULL_HANDLE, &image_index);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        refresh();
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("Failed to acquire swapchain image");
    }

    if (images_in_flight[image_index] != VK_NULL_HANDLE)
    {
        vkWaitForFences(device, 1, &images_in_flight[image_index], VK_TRUE, UINT64_MAX);
    }

    images_in_flight[image_index] = in_flight_fences[current_frame];

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = {image_available_semaphores[current_frame]};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffers[image_index];

    VkSemaphore signal_semaphores[] = {render_finished_semaphores[current_frame]};
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(device, 1, &in_flight_fences[current_frame]);

    VK_CHECK(vkQueueSubmit(device.graphics_queue(), 1, &submit_info, in_flight_fences[current_frame]));

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;
    VkSwapchainKHR swapchains[] = {swapchain};
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swapchains;
    present_info.pImageIndices = &image_index;
    present_info.pResults = nullptr;

    result = vkQueuePresentKHR(device.present_queue(), &present_info);

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

void VkContext::wait_idle()
{
    vkDeviceWaitIdle(device);
}
