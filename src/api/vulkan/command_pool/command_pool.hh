#pragma once

#include <vulkan/vulkan.h>

#include "command_buffer/command_buffer.hh"

class CommandPool
{
public:
    void create();
    void destroy();
    void reset();

    CommandBuffer allocate_command_buffer() const;

    inline VkCommandPool handle() const
    {
        return handle_;
    }

    inline operator VkCommandPool() const
    {
        return handle_;
    }

private:
    VkCommandPool handle_ = VK_NULL_HANDLE;
};
