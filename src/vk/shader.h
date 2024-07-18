#pragma once

#include <volk.h>

namespace vk
{
struct shader
{
    VkShaderModule vk_handle = VK_NULL_HANDLE;
    const char* path;
};
} // namespace vk
