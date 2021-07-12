#pragma once

#include <string>

#include <vulkan/vulkan.h>

namespace vk
{
    struct Shader
    {
        std::string path;
        VkShaderModule vk_handle = VK_NULL_HANDLE;
    };
}
