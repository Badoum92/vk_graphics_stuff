#pragma once

#include <vulkan/vulkan.h>

class CommandBuffer
{
public:
    void create(VkCommandPool pool);
    void destroy();

    void record(size_t image_index);

    inline VkCommandBuffer& handle()
    {
        return handle_;
    }

    inline operator VkCommandBuffer&()
    {
        return handle_;
    }

private:
    VkCommandBuffer handle_;
    VkCommandPool pool_;
};
