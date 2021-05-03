#pragma once

#include <vulkan/vulkan.h>

class Sampler
{
public:
    ~Sampler();

    void create();
    void destroy();

    inline const VkSampler& handle() const
    {
        return handle_;
    }

    inline operator const VkSampler&() const
    {
        return handle_;
    }

private:
    VkSampler handle_;
};
