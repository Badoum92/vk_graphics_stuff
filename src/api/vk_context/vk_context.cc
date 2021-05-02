#include "vk_context.hh"

#include <vma/vk_mem_alloc.h>
#include <stdexcept>
#include <iostream>

#include "window/window.hh"
#include "vertex.hh"

size_t VkContext::current_frame = 0;
std::array<FrameData, VkContext::FRAMES_IN_FLIGHT> VkContext::frames;

Instance VkContext::instance;
Surface VkContext::surface;
PhysicalDevice VkContext::physical_device;
Device VkContext::device;
CommandPool VkContext::command_pool;
DescriptorPool VkContext::descriptor_pool;
SwapChain VkContext::swapchain;
RenderPass VkContext::renderpass;
Pipeline VkContext::pipeline;
std::vector<FrameBuffer> VkContext::framebuffers;

VmaAllocator VkContext::allocator;

/* --- */

Buffer VkContext::vertex_buffer;
Buffer VkContext::index_buffer;
Buffer VkContext::global_uniform_buffer;

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

    inner_create();
}

void VkContext::destroy()
{
    inner_destroy();

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
    renderpass.create();
    pipeline.create();

    framebuffers.resize(swapchain.size());
    for (size_t i = 0; i < framebuffers.size(); ++i)
    {
        framebuffers[i].create(renderpass, swapchain.image_views()[i]);
    }

    for (size_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
    {
        frames[i].create();
    }

    // clang-format off
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };

    const std::vector<uint16_t> indices = {
        0, 1, 2,
        2, 3, 0
    };
    // clang-format on
    vertex_buffer.create_vertex(vertices.data(), vertices.size() * sizeof(Vertex));
    index_buffer.create_index(indices.data(), indices.size() * sizeof(uint16_t));
    global_uniform_buffer.create_uniform(4 * KB);

    DescriptorSet::global_set.bind_buffer(0, global_uniform_buffer, 16 * sizeof(float));
    DescriptorSet::global_set.update();
}

void VkContext::inner_destroy()
{
    DescriptorSet::destroy_global_set();

    for (size_t i = 0; i < FRAMES_IN_FLIGHT; ++i)
    {
        frames[i].destroy();
    }

    for (auto& framebuffer : framebuffers)
    {
        framebuffer.destroy();
    }

    pipeline.destroy();
    renderpass.destroy();
    swapchain.destroy();

    vertex_buffer.destroy();
    index_buffer.destroy();
    global_uniform_buffer.destroy();
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

void VkContext::draw_frame()
{
    vkWaitForFences(device, 1, &frames[current_frame].render_fence, VK_TRUE, UINT64_MAX);

    uint32_t image_index;
    VkResult result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, frames[current_frame].render_semaphore,
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

    // if (images_in_flight[image_index] != VK_NULL_HANDLE)
    // {
    //     vkWaitForFences(device, 1, &images_in_flight[image_index], VK_TRUE, UINT64_MAX);
    // }
    // images_in_flight[image_index] = in_flight_fences[current_frame];

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    frames[current_frame].begin_frame();
    frames[current_frame].cmd_buffer.record(image_index);

    VkSemaphore wait_semaphores[] = {frames[current_frame].render_semaphore};
    VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
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
