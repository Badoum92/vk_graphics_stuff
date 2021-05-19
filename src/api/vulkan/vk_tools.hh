#pragma once

#include <vulkan/vulkan.h>
#include <functional>
#include <stdexcept>
#include <string>

bool has_stencil_component(VkFormat format);
bool has_depth_component(VkFormat format);
VkFormat get_depth_format();
void vk_execute_once(const std::function<void(VkCommandBuffer)>& f);

const char* vk_result_to_str(VkResult result);

#define VK_CHECK(RESULT)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        VkResult result__ = (RESULT);                                                                                  \
        if (result__ != VK_SUCCESS)                                                                                    \
        {                                                                                                              \
            std::string error_str__ = __func__;                                                                        \
            error_str__ += " (";                                                                                       \
            error_str__ += std::to_string(__LINE__);                                                                   \
            error_str__ += "): ";                                                                                      \
            error_str__ += vk_result_to_str(result__);                                                                 \
            throw std::runtime_error(error_str__);                                                                       \
        }                                                                                                              \
    } while (0)

#define KB (1 << 10)
#define MB (1 << 20)
