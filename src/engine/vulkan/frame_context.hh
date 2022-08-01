#pragma once

#include <vector>

#include <volk.h>

#include "handle.hh"
#include "command.hh"

namespace vk
{
struct FrameBuffer;
struct Image;

struct FrameContext
{
    VkSemaphore image_acquired_semaphore = VK_NULL_HANDLE;
    VkSemaphore rendering_finished_semaphore = VK_NULL_HANDLE;
    VkFence rendering_finished_fence = VK_NULL_HANDLE;
    uint32_t image_index = 0;
    CommandContext command_context;

    Handle<FrameBuffer> framebuffer;
    Handle<Image> image;
};
}
