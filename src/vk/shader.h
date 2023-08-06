#pragma once

#include <volk.h>
#include <string>

namespace vk
{
struct Shader
{
    std::string path;
    VkShaderModule vk_handle = VK_NULL_HANDLE;
};
} // namespace vk
