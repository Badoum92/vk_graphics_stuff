#pragma once

#include <vector>

#include <volk.h>

#include "fwd.h"
#include "bul/containers/handle.h"
#include "command.h"

namespace vk
{
struct FrameContext
{
    VkSemaphore image_acquired_semaphore = VK_NULL_HANDLE;
    VkSemaphore rendering_finished_semaphore = VK_NULL_HANDLE;
    VkFence rendering_finished_fence = VK_NULL_HANDLE;
    uint32_t image_index = 0;
    CommandContext command_context;

    bul::handle<FrameBuffer> framebuffer;
    bul::handle<Image> image;
};
}
