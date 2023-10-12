#pragma once

#include <volk.h>
#include <string>
#include <string_view>

#include "vk/image.h"

#include "bul/log.h"
#include "bul/bul.h"

namespace vk
{
struct context;

image_access get_src_image_access(image_usage usage);
image_access get_dst_image_access(image_usage usage);
VkImageMemoryBarrier get_image_barrier(image& image, const image_access& src, const image_access& dst);

bool is_depth(VkFormat format);
bool is_stencil(VkFormat format);

const char* vk_result_to_str(VkResult result);

void set_resource_name(context* context, uint64_t vk_handle, VkDebugReportObjectTypeEXT objectType,
                       std::string_view name);

#define VK_CHECK(RESULT)                                                                                               \
    do                                                                                                                 \
    {                                                                                                                  \
        VkResult _result__ = (RESULT);                                                                                 \
        ASSERT_FMT(_result__ == VK_SUCCESS, "%s (%s)", #RESULT, vk_result_to_str(_result__));                          \
    } while (0)

} // namespace vk
