#pragma once

#include <volk.h>
#include <string>
#include <stdexcept>

#include "image.h"

namespace vk
{
ImageAccess get_src_image_access(ImageUsage usage);
ImageAccess get_dst_image_access(ImageUsage usage);
VkImageMemoryBarrier get_image_barrier(Image& image, const ImageAccess& src, const ImageAccess& dst);

bool is_depth(VkFormat format);
bool is_stencil(VkFormat format);

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
            throw std::runtime_error(error_str__);                                                                     \
        }                                                                                                              \
    } while (0)

#define KB (1 << 10)
#define MB (1 << 20)
} // namespace vk
