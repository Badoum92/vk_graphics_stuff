#pragma once

#include <string>

#include <volk.h>

namespace vk
{
    struct Shader
    {
        std::string path;
        VkShaderModule vk_handle = VK_NULL_HANDLE;
    };
}
