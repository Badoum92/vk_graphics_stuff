#include "vk_tools.h"

#include "vk/context.h"

#include "bul/bul.h"

namespace vk
{
ImageAccess get_src_image_access(ImageUsage usage)
{
    ImageAccess access;
    switch (usage)
    {
    case ImageUsage::GraphicsShaderRead:
        access.stage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
        access.access = 0;
        access.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        break;
    case ImageUsage::GraphicsShaderReadWrite:
        access.stage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        access.access = VK_ACCESS_SHADER_WRITE_BIT;
        access.layout = VK_IMAGE_LAYOUT_GENERAL;
        break;
    case ImageUsage::ComputeShaderRead:
        access.stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        access.access = 0;
        access.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        break;
    case ImageUsage::ComputeShaderReadWrite:
        access.stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        access.access = VK_ACCESS_SHADER_WRITE_BIT;
        access.layout = VK_IMAGE_LAYOUT_GENERAL;
        break;
    case ImageUsage::TransferDst:
        access.stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        access.access = VK_ACCESS_TRANSFER_WRITE_BIT;
        access.layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        break;
    case ImageUsage::TransferSrc:
        access.stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        access.access = 0;
        access.layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        break;
    case ImageUsage::ColorAttachment:
        access.stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        access.access = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        access.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        break;
    case ImageUsage::DepthAttachment:
        access.stage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        access.access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        access.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        break;
    case ImageUsage::Present:
        access.stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        access.access = 0;
        access.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        break;
    case ImageUsage::None:
        access.stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        access.access = 0;
        access.layout = VK_IMAGE_LAYOUT_UNDEFINED;
        break;
    default:
        ASSERT(false);
        break;
    };
    return access;
}

ImageAccess get_dst_image_access(ImageUsage usage)
{
    ImageAccess access;
    switch (usage)
    {
    case ImageUsage::GraphicsShaderRead:
        access.stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        access.access = VK_ACCESS_SHADER_READ_BIT;
        access.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        break;
    case ImageUsage::GraphicsShaderReadWrite:
        access.stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        access.access = VK_ACCESS_SHADER_WRITE_BIT;
        access.layout = VK_IMAGE_LAYOUT_GENERAL;
        break;
    case ImageUsage::ComputeShaderRead:
        access.stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        access.access = VK_ACCESS_SHADER_READ_BIT;
        access.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        break;
    case ImageUsage::ComputeShaderReadWrite:
        access.stage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        access.access = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        access.layout = VK_IMAGE_LAYOUT_GENERAL;
        break;
    case ImageUsage::TransferDst:
        access.stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        access.access = VK_ACCESS_TRANSFER_WRITE_BIT;
        access.layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        break;
    case ImageUsage::TransferSrc:
        access.stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        access.access = VK_ACCESS_TRANSFER_READ_BIT;
        access.layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        break;
    case ImageUsage::ColorAttachment:
        access.stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        access.access = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        access.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        break;
    case ImageUsage::DepthAttachment:
        access.stage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        access.access = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        access.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        break;
    case ImageUsage::Present:
        access.stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        access.access = 0;
        access.layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        break;
    case ImageUsage::None:
        access.stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        access.access = 0;
        access.layout = VK_IMAGE_LAYOUT_UNDEFINED;
        break;
    default:
        ASSERT(false);
        break;
    };
    return access;
}

VkImageMemoryBarrier get_image_barrier(Image& image, const ImageAccess& src, const ImageAccess& dst)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = src.layout;
    barrier.newLayout = dst.layout;
    barrier.srcAccessMask = src.access;
    barrier.dstAccessMask = dst.access;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image.vk_handle;
    barrier.subresourceRange = image.full_view.range;
    return barrier;
}

bool is_stencil(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

bool is_depth(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT || format == VK_FORMAT_D32_SFLOAT_S8_UINT
        || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void set_resource_name(Context* context, uint64_t vk_handle, VkDebugReportObjectTypeEXT objectType,
                       std::string_view name)
{
    VkDebugMarkerObjectNameInfoEXT name_info{};
    name_info.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_OBJECT_NAME_INFO_EXT;
    name_info.objectType = objectType;
    name_info.object = vk_handle;
    name_info.pObjectName = name.data();
    vkDebugMarkerSetObjectNameEXT(context->device, &name_info);
}

const char* vk_result_to_str(VkResult result)
{
#define CASE(ARG)                                                                                                      \
    case ARG:                                                                                                          \
        return #ARG
    switch (result)
    {
        CASE(VK_SUCCESS);
        CASE(VK_NOT_READY);
        CASE(VK_TIMEOUT);
        CASE(VK_EVENT_SET);
        CASE(VK_EVENT_RESET);
        CASE(VK_INCOMPLETE);
        CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
        CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
        CASE(VK_ERROR_INITIALIZATION_FAILED);
        CASE(VK_ERROR_DEVICE_LOST);
        CASE(VK_ERROR_MEMORY_MAP_FAILED);
        CASE(VK_ERROR_LAYER_NOT_PRESENT);
        CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
        CASE(VK_ERROR_FEATURE_NOT_PRESENT);
        CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
        CASE(VK_ERROR_TOO_MANY_OBJECTS);
        CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
        CASE(VK_ERROR_FRAGMENTED_POOL);
        CASE(VK_ERROR_UNKNOWN);
        CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
        CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
        CASE(VK_ERROR_FRAGMENTATION);
        CASE(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
        CASE(VK_ERROR_SURFACE_LOST_KHR);
        CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
        CASE(VK_SUBOPTIMAL_KHR);
        CASE(VK_ERROR_OUT_OF_DATE_KHR);
        CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
        CASE(VK_ERROR_VALIDATION_FAILED_EXT);
        CASE(VK_ERROR_INVALID_SHADER_NV);
        CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        CASE(VK_ERROR_NOT_PERMITTED_EXT);
        CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        CASE(VK_THREAD_IDLE_KHR);
        CASE(VK_THREAD_DONE_KHR);
        CASE(VK_OPERATION_DEFERRED_KHR);
        CASE(VK_OPERATION_NOT_DEFERRED_KHR);
        CASE(VK_PIPELINE_COMPILE_REQUIRED_EXT);
        CASE(VK_RESULT_MAX_ENUM);
    }
#undef CASE
}
} // namespace vk
