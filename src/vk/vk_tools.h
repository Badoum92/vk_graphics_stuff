#pragma once

#include <volk.h>
#include <string>
#include <string_view>

#include "vk/image.h"

#include "bul/log.h"
#include "bul/bul.h"

namespace vk
{
struct Context;

ImageAccess get_src_image_access(ImageUsage usage);
ImageAccess get_dst_image_access(ImageUsage usage);
VkImageMemoryBarrier get_image_barrier(Image& image, const ImageAccess& src, const ImageAccess& dst);

bool is_depth(VkFormat format);
bool is_stencil(VkFormat format);

const char* vk_result_to_str(VkResult result);

void set_resource_name(Context* context, uint64_t vk_handle, VkDebugReportObjectTypeEXT objectType,
                       std::string_view name);

#define VK_CHECK(RESULT) ASSERT_FMT((RESULT) == VK_SUCCESS, "%s", vk_result_to_str(result__))

} // namespace vk