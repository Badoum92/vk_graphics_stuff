#include "vk/vk_commands.h"

#include "vk/vk_context.h"
#include "vk/vk_image.h"
#include "vk/vk_buffer.h"
#include "vk/vk_pipeline.h"
#include "vk/vk_constants.h"
#include "vk/vk_tools.h"

namespace vk
{
void command_buffer::begin()
{
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    VK_CHECK(vkBeginCommandBuffer(vk_handle, &begin_info));
}

void command_buffer::end()
{
    VK_CHECK(vkEndCommandBuffer(vk_handle));
}

void command_buffer::bind_descriptor_buffer(graphics_pipeline* pipeline)
{
    VkDescriptorBufferBindingInfoEXT descriptor_buffer_binding_infos[2] = {};
    descriptor_buffer_binding_infos[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
    descriptor_buffer_binding_infos[0].address = context->texture_descriptor_set.buffer->device_address;
    descriptor_buffer_binding_infos[0].usage = texture_descriptor_buffer_usage;
    descriptor_buffer_binding_infos[1].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
    descriptor_buffer_binding_infos[1].address = context->image_descriptor_set.buffer->device_address;
    descriptor_buffer_binding_infos[1].usage = image_descriptor_buffer_usage;
    vkCmdBindDescriptorBuffersEXT(vk_handle, ARRAY_SIZE(descriptor_buffer_binding_infos),
                                  descriptor_buffer_binding_infos);

    VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
    uint32_t index = 0;
    VkDeviceSize offset = 0;
    vkCmdSetDescriptorBufferOffsetsEXT(vk_handle, bind_point, pipeline->layout, 0, 1, &index, &offset);
    index++;
    offset += context->texture_descriptor_set.size;
    vkCmdSetDescriptorBufferOffsetsEXT(vk_handle, bind_point, pipeline->layout, 1, 1, &index, &offset);
}

void command_buffer::bind_descriptor_buffer(compute_pipeline* pipeline)
{
    VkDescriptorBufferBindingInfoEXT descriptor_buffer_binding_infos[2] = {};
    descriptor_buffer_binding_infos[0].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
    descriptor_buffer_binding_infos[0].address = context->texture_descriptor_set.buffer->device_address;
    descriptor_buffer_binding_infos[0].usage = texture_descriptor_buffer_usage;
    descriptor_buffer_binding_infos[1].sType = VK_STRUCTURE_TYPE_DESCRIPTOR_BUFFER_BINDING_INFO_EXT;
    descriptor_buffer_binding_infos[1].address = context->image_descriptor_set.buffer->device_address;
    descriptor_buffer_binding_infos[1].usage = image_descriptor_buffer_usage;
    vkCmdBindDescriptorBuffersEXT(vk_handle, ARRAY_SIZE(descriptor_buffer_binding_infos),
                                  descriptor_buffer_binding_infos);

    VkPipelineBindPoint bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
    uint32_t index = 0;
    VkDeviceSize offset = 0;
    vkCmdSetDescriptorBufferOffsetsEXT(vk_handle, bind_point, pipeline->layout, 0, 1, &index, &offset);
    // index = 1; // for some reason this is does not work for compute shaders ?
    offset += context->texture_descriptor_set.size;
    vkCmdSetDescriptorBufferOffsetsEXT(vk_handle, bind_point, pipeline->layout, 1, 1, &index, &offset);
}

void command_buffer::begin_rendering(bul::span<image*> color_attachments, bul::span<load_op> color_load_ops,
                                     image* depth_attachment, load_op depth_load_op)
{
    ASSERT(color_attachments.size <= max_color_attachments);
    ASSERT(color_attachments.size == color_load_ops.size);

    VkRenderingAttachmentInfo color_attachment_infos[max_color_attachments];

    for (uint32_t i = 0; i < color_attachments.size; ++i)
    {
        VkRenderingAttachmentInfo& attachment_info = color_attachment_infos[i];
        memset(&attachment_info, 0, sizeof(attachment_info));
        attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        attachment_info.pNext = nullptr;
        attachment_info.imageView = color_attachments[i]->full_view.vk_handle;
        attachment_info.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
        attachment_info.loadOp = color_load_ops[i].vk_loadop;
        attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_info.clearValue = color_load_ops[i].clear_value;
    }

    VkRenderingAttachmentInfo depth_attachment_info = {};
    if (depth_attachment)
    {
        depth_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        depth_attachment_info.pNext = nullptr;
        depth_attachment_info.imageView = depth_attachment->full_view.vk_handle;
        depth_attachment_info.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        depth_attachment_info.loadOp = depth_load_op.vk_loadop;
        depth_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment_info.clearValue = depth_load_op.clear_value;
    }

    VkRenderingInfo rendering_info = {};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.renderArea.offset = {0, 0};
    rendering_info.renderArea.extent = {color_attachments[0]->description.width,
                                        color_attachments[0]->description.height};
    rendering_info.layerCount = 1;
    rendering_info.colorAttachmentCount = color_attachments.size;
    rendering_info.pColorAttachments = color_attachment_infos;
    rendering_info.pDepthAttachment = depth_attachment ? &depth_attachment_info : nullptr;

    vkCmdBeginRendering(vk_handle, &rendering_info);
}

void command_buffer::end_rendering()
{
    vkCmdEndRendering(vk_handle);
}

void command_buffer::push_constant(graphics_pipeline* pipeline, void* data, uint32_t size)
{
    vkCmdPushConstants(vk_handle, pipeline->layout, VK_SHADER_STAGE_ALL, 0, size, data);
}

void command_buffer::push_constant(compute_pipeline* pipeline, void* data, uint32_t size)
{
    vkCmdPushConstants(vk_handle, pipeline->layout, VK_SHADER_STAGE_ALL, 0, size, data);
}

void command_buffer::bind_index_buffer(buffer* buffer, uint32_t offset, VkIndexType index_type)
{
    vkCmdBindIndexBuffer(vk_handle, buffer->vk_handle, offset, index_type);
}

void command_buffer::bind_graphics_pipeline(graphics_pipeline* pipeline, const graphics_state& graphics_state)
{
    VkPipeline vk_pipeline_handle = context->compile_graphics_pipeline(pipeline, graphics_state);
    vkCmdBindPipeline(vk_handle, VK_PIPELINE_BIND_POINT_GRAPHICS, vk_pipeline_handle);
}

void command_buffer::bind_compute_pipeline(compute_pipeline* pipeline)
{
    vkCmdBindPipeline(vk_handle, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->vk_handle);
}

void command_buffer::set_scissor(const VkRect2D& rect)
{
    vkCmdSetScissor(vk_handle, 0, 1, &rect);
}

void command_buffer::set_viewport(const VkViewport& viewport)
{
    vkCmdSetViewport(vk_handle, 0, 1, &viewport);
}

void command_buffer::draw(uint32_t vertex_count, uint32_t first_vertex)
{
    vkCmdDraw(vk_handle, vertex_count, 1, first_vertex, 0);
}

void command_buffer::draw_indexed(uint32_t index_count, uint32_t first_index, uint32_t vertex_offset)
{
    vkCmdDrawIndexed(vk_handle, index_count, 1, first_index, vertex_offset, 0);
}

void command_buffer::compute_dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    vkCmdDispatch(vk_handle, x, y, z);
}

command_pool command_pool::create(vk::context* context, uint32_t queue_index, VkQueue vk_queue)
{
    command_pool command_pool;
    command_pool.context = context;
    command_pool.vk_queue = vk_queue;

    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = queue_index;
    pool_info.flags = 0;

    VK_CHECK(vkCreateCommandPool(context->device, &pool_info, nullptr, &command_pool.vk_handle));

    return command_pool;
}

void command_buffer::barrier(image* image, const image_usage& dst_usage)
{
    image_access src_access = get_src_image_access(image->usage);
    image_access dst_access = get_dst_image_access(dst_usage);
    auto barrier = get_image_barrier(image, src_access, dst_access);
    vkCmdPipelineBarrier(vk_handle, src_access.stage, dst_access.stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    image->usage = dst_usage;
}

void command_buffer::copy_buffer_to_buffer(buffer* src, buffer* dst, uint32_t size, uint32_t src_offset,
                                           uint32_t dst_offset)
{
    VkBufferCopy buffer_copy = {};
    buffer_copy.srcOffset = src_offset;
    buffer_copy.dstOffset = dst_offset;
    buffer_copy.size = size;

    vkCmdCopyBuffer(vk_handle, src->vk_handle, dst->vk_handle, 1, &buffer_copy);
}

void command_buffer::copy_image_to_buffer(image* image, buffer* buffer, uint32_t dst_offset)
{
    VkBufferImageCopy buffer_image_copy = {};
    buffer_image_copy.bufferOffset = dst_offset;
    buffer_image_copy.bufferRowLength = 0;
    buffer_image_copy.bufferImageHeight = 0;
    buffer_image_copy.imageSubresource.aspectMask =
        image->usage == image_usage::depth_attachment ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
    buffer_image_copy.imageSubresource.mipLevel = 0;
    buffer_image_copy.imageSubresource.baseArrayLayer = 0;
    buffer_image_copy.imageSubresource.layerCount = 1;
    buffer_image_copy.imageOffset = {0, 0, 0};
    buffer_image_copy.imageExtent = {image->description.width, image->description.height, image->description.depth};

    barrier(image, vk::image_usage::transfer_src);

    vkCmdCopyImageToBuffer(vk_handle, image->vk_handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer->vk_handle, 1,
                           &buffer_image_copy);
}

void command_buffer::copy_buffer_to_image(buffer* buffer, image* image, uint32_t src_offset)
{
    VkBufferImageCopy buffer_image_copy = {};
    buffer_image_copy.bufferOffset = src_offset;
    buffer_image_copy.bufferRowLength = 0;
    buffer_image_copy.bufferImageHeight = 0;
    buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    buffer_image_copy.imageSubresource.mipLevel = 0;
    buffer_image_copy.imageSubresource.baseArrayLayer = 0;
    buffer_image_copy.imageSubresource.layerCount = 1;
    buffer_image_copy.imageOffset = {0, 0, 0};
    buffer_image_copy.imageExtent = {image->description.width, image->description.height, image->description.depth};

    barrier(image, vk::image_usage::transfer_dst);

    vkCmdCopyBufferToImage(vk_handle, buffer->vk_handle, image->vk_handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
                           &buffer_image_copy);
}

void command_buffer::upload_buffer(buffer* buffer, ::vk::buffer* staging_buffer, void* data, uint32_t size,
                                   uint32_t src_offset, uint32_t dst_offset)
{
    memcpy(staging_buffer->mapped_data, data, size);
    copy_buffer_to_buffer(staging_buffer, buffer, size, src_offset, dst_offset);
}

void command_buffer::upload_image(image* image, buffer* staging_buffer, void* data, uint32_t size)
{
    memcpy(staging_buffer->mapped_data, data, size);
    copy_buffer_to_image(staging_buffer, image);
}

void command_pool::destroy()
{
    reset();
    vkDestroyCommandPool(context->device, vk_handle, nullptr);
    vk_handle = VK_NULL_HANDLE;
    vk_queue = VK_NULL_HANDLE;
    command_buffers.destroy();
}

void command_pool::reset()
{
    VK_CHECK(vkResetCommandPool(context->device, vk_handle, 0));
    current_index = 0;
}

command_buffer* command_pool::get_command_buffer()
{
    if (current_index == command_buffers.size)
    {
        command_buffer& command_buffer = command_buffers.push_back();
        VkCommandBufferAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        alloc_info.commandPool = vk_handle;
        alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        alloc_info.commandBufferCount = 1;
        VK_CHECK(vkAllocateCommandBuffers(context->device, &alloc_info, &command_buffer.vk_handle));
        command_buffer.wait_stage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    }
    command_buffer* command_buffer = &command_buffers[current_index++];
    command_buffer->context = context;
    command_buffer->vk_queue = vk_queue;
    command_buffer->begin();
    return command_buffer;
}
} // namespace vk
