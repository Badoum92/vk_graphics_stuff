#pragma once

#include <vulkan/vulkan.h>
#include <vector>

class CommandBuffers
{
public:
    void create();
    void destroy();

    size_t size() const;
    void record(size_t i);

    inline const VkCommandBuffer& operator[](size_t i)
    {
        return handles_[i];
    }

private:
    std::vector<VkCommandBuffer> handles_;
};
