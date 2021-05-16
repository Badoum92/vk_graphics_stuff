#pragma once

#include <vulkan/vulkan.h>

#include "command_pool/command_pool.hh"
#include "command_buffer/command_buffer.hh"

struct FrameData
{
    void create();
    void destroy();

    void begin_frame();
    void end_frame();

    static FrameData& current();

    VkSemaphore render_semaphore;
    VkSemaphore present_semaphore;
    VkFence render_fence;

    CommandPool command_pool;
    CommandBuffer cmd;
};
