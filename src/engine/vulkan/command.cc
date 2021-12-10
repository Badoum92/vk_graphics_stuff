#include "command.hh"

#include <stb/stb_image.h>

#include "vk_tools.hh"
#include "descriptor_set.hh"
#include "graphics_pipeline.hh"
#include "framebuffer.hh"
#include "device.hh"

namespace vk
{
GraphicsCommand& Device::get_graphics_command()
{
    auto& fc = frame_contexts[current_frame];
    return fc.command_context.graphics_pool.get_command();
}

ComputeCommand& Device::get_compute_command()
{
    auto& fc = frame_contexts[current_frame];
    return fc.command_context.compute_pool.get_command();
}

TransferCommand& Device::get_transfer_command()
{
    auto& fc = frame_contexts[current_frame];
    return fc.command_context.transfer_pool.get_command();
}

void Device::submit(Command& command, VkSemaphore wait_semaphore, VkSemaphore signal_semaphore, VkFence fence)
{
    command.end();
    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command.vk_handle;
    if (wait_semaphore != VK_NULL_HANDLE)
    {
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &wait_semaphore;
        submit_info.pWaitDstStageMask = &command.wait_stage;
    }
    if (signal_semaphore != VK_NULL_HANDLE)
    {
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &signal_semaphore;
    }
    if (fence != VK_NULL_HANDLE)
    {
        vkResetFences(vk_handle, 1, &fence);
    }
    vkQueueSubmit(command.vk_queue, 1, &submit_info, fence);
}

void Device::submit(Command& command)
{
    submit(command, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE);
}

void Device::submit_blocking(Command& command)
{
    submit(command, VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE);
    wait_idle();
}

/* CommandContext */

CommandContext CommandContext::create(Device& device)
{
    CommandContext command_context;
    command_context.p_device = &device;
    command_context.graphics_pool = CommandPool<GraphicsCommand>::create(device);
    command_context.compute_pool = CommandPool<ComputeCommand>::create(device);
    command_context.transfer_pool = CommandPool<TransferCommand>::create(device);
    return command_context;
}

void CommandContext::destroy()
{
    graphics_pool.destroy();
    compute_pool.destroy();
    transfer_pool.destroy();
    p_device = nullptr;
}

void CommandContext::reset()
{
    graphics_pool.reset();
    compute_pool.reset();
    transfer_pool.reset();
}

/* Commands */

void Command::begin()
{
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(vk_handle, &begin_info);
}

void Command::end()
{
    vkEndCommandBuffer(vk_handle);
}

void Command::barrier(const Handle<Image>& image_handle, ImageUsage dst_usage)
{
    auto& image = p_device->images.get(image_handle);
    const auto& src_access = get_src_image_access(image.usage);
    const auto& dst_access = get_dst_image_access(dst_usage);
    auto barrier = get_image_barrier(image, src_access, dst_access);
    vkCmdPipelineBarrier(vk_handle, src_access.stage, dst_access.stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    image.usage = dst_usage;
}

void Command::bind_image(const Handle<GraphicsProgram>& program_handle, const Handle<Image>& image_handle,
                         uint32_t binding)
{
    auto& program = p_device->graphics_programs.get(program_handle);
    program.descriptor_set.bind_image(binding, image_handle);
}

void Command::bind_uniform_buffer(const Handle<GraphicsProgram>& program_handle, const Handle<Buffer>& buffer_handle,
                                  uint32_t binding, uint32_t offset, uint32_t size)
{
    auto& program = p_device->graphics_programs.get(program_handle);
    program.descriptor_set.bind_uniform_buffer(binding, buffer_handle, offset, size);
}

void Command::bind_storage_buffer(const Handle<GraphicsProgram>& program_handle, const Handle<Buffer>& buffer_handle,
                                  uint32_t binding)
{
    auto& program = p_device->graphics_programs.get(program_handle);
    program.descriptor_set.bind_storage_buffer(binding, buffer_handle);
}

void Command::bind_image(const Handle<ComputeProgram>& program_handle, const Handle<Image>& image_handle,
                         uint32_t binding)
{
    auto& program = p_device->compute_programs.get(program_handle);
    program.descriptor_set.bind_image(binding, image_handle);
}

void Command::bind_uniform_buffer(const Handle<ComputeProgram>& program_handle, const Handle<Buffer>& buffer_handle,
                                  uint32_t binding, uint32_t offset, uint32_t size)
{
    auto& program = p_device->compute_programs.get(program_handle);
    program.descriptor_set.bind_uniform_buffer(binding, buffer_handle, offset, size);
}

void Command::bind_storage_buffer(const Handle<ComputeProgram>& program_handle, const Handle<Buffer>& buffer_handle,
                                  uint32_t binding)
{
    auto& program = p_device->compute_programs.get(program_handle);
    program.descriptor_set.bind_storage_buffer(binding, buffer_handle);
}

/* Graphics */

void GraphicsCommand::set_scissor(const VkRect2D& rect)
{
    vkCmdSetScissor(vk_handle, 0, 1, &rect);
}

void GraphicsCommand::set_viewport(const VkViewport& viewport)
{
    vkCmdSetViewport(vk_handle, 0, 1, &viewport);
}

void GraphicsCommand::bind_index_buffer(const Handle<Buffer>& buffer_handle, VkIndexType index_type, uint32_t offset)
{
    auto& buffer = p_device->buffers.get(buffer_handle);
    vkCmdBindIndexBuffer(vk_handle, buffer.vk_handle, offset, index_type);
}

void GraphicsCommand::bind_descriptor_set(const Handle<GraphicsProgram>& program_handle, DescriptorSet& set,
                                          uint32_t set_index)
{
    auto& program = p_device->graphics_programs.get(program_handle);
    auto vk_set = set.get_or_create_vk_set(*p_device);
    vkCmdBindDescriptorSets(vk_handle, VK_PIPELINE_BIND_POINT_GRAPHICS, program.layout, set_index, 1, &vk_set,
                            set.dynamic_offsets.size(), set.dynamic_offsets.data());
}

void GraphicsCommand::bind_pipeline(const Handle<GraphicsProgram>& program_handle, uint32_t pipeline_index)
{
    auto& program = p_device->graphics_programs.get(program_handle);
    if (program.description.descriptor_types.size() > 0)
    {
        bind_descriptor_set(program_handle, program.descriptor_set, 1);
    }
    vkCmdBindPipeline(vk_handle, VK_PIPELINE_BIND_POINT_GRAPHICS, program.pipelines[pipeline_index]);
}

void GraphicsCommand::begin_renderpass(const Handle<FrameBuffer>& framebuffer_handle,
                                       const std::vector<LoadOp>& load_ops)
{
    auto& framebuffer = p_device->framebuffers.get(framebuffer_handle);
    const auto& renderpass = p_device->get_or_create_renderpass(framebuffer_handle, load_ops);

    static std::vector<VkClearValue> clear_values;
    clear_values.clear();
    for (const auto& load_op : renderpass.load_ops)
    {
        clear_values.push_back(load_op.clear_value);
    }

    VkRenderPassBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    begin_info.pNext = nullptr;
    begin_info.renderPass = renderpass.vk_handle;
    begin_info.framebuffer = framebuffer.vk_handle;
    begin_info.renderArea.offset = {0, 0};
    begin_info.renderArea.extent = {framebuffer.description.width, framebuffer.description.height};
    begin_info.clearValueCount = clear_values.size();
    begin_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(vk_handle, &begin_info, VK_SUBPASS_CONTENTS_INLINE);
}

void GraphicsCommand::end_renderpass()
{
    vkCmdEndRenderPass(vk_handle);
}

void GraphicsCommand::draw(uint32_t vertex_count, uint32_t first_vertex)
{
    vkCmdDraw(vk_handle, vertex_count, 1, first_vertex, 0);
}

void GraphicsCommand::draw_indexed(uint32_t index_count, uint32_t first_index, uint32_t vertex_offset)
{
    vkCmdDrawIndexed(vk_handle, index_count, 1, first_index, vertex_offset, 0);
}

/* Compute */

void ComputeCommand::bind_descriptor_set(const Handle<ComputeProgram>& program_handle, DescriptorSet& set,
                                         uint32_t set_index)
{
    auto& program = p_device->compute_programs.get(program_handle);
    auto vk_set = set.get_or_create_vk_set(*p_device);
    vkCmdBindDescriptorSets(vk_handle, VK_PIPELINE_BIND_POINT_COMPUTE, program.layout, set_index, 1, &vk_set,
                            set.dynamic_offsets.size(), set.dynamic_offsets.data());
}

void ComputeCommand::bind_pipeline(const Handle<ComputeProgram>& program_handle)
{
    auto& program = p_device->compute_programs.get(program_handle);
    if (program.description.descriptor_types.size() > 0)
    {
        bind_descriptor_set(program_handle, program.descriptor_set, 1);
    }
    vkCmdBindPipeline(vk_handle, VK_PIPELINE_BIND_POINT_COMPUTE, program.pipeline);
}

void ComputeCommand::dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    vkCmdDispatch(vk_handle, x, y, z);
}

/* Transfer */

void TransferCommand::upload_buffer(const Handle<Buffer>& buffer_handle, void* data, uint32_t size)
{
    // FIXME delete buffer after use
    auto staging_handle = p_device->create_buffer(
        {.size = size, .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT, .memory_usage = VMA_MEMORY_USAGE_CPU_ONLY});

    void* staging_area = p_device->map_buffer(staging_handle);
    std::memcpy(staging_area, data, size);
    p_device->unmap_buffer(staging_handle);

    auto& buffer = p_device->buffers.get(buffer_handle);
    auto& staging_buffer = p_device->buffers.get(staging_handle);

    assert(buffer.description.size == size);

    VkBufferCopy buffer_copy{};
    buffer_copy.srcOffset = 0;
    buffer_copy.dstOffset = 0;
    buffer_copy.size = size;
    vkCmdCopyBuffer(vk_handle, staging_buffer.vk_handle, buffer.vk_handle, 1, &buffer_copy);
}

void TransferCommand::upload_image(const Handle<Image>& image_handle, void* data, uint32_t size)
{
    // FIXME delete buffer after use
    auto staging_handle = p_device->create_buffer(
        {.size = size, .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT, .memory_usage = VMA_MEMORY_USAGE_CPU_ONLY});

    void* staging_area = p_device->map_buffer(staging_handle);
    std::memcpy(staging_area, data, size);
    p_device->unmap_buffer(staging_handle);

    auto& image = p_device->images.get(image_handle);
    auto& staging_buffer = p_device->buffers.get(staging_handle);

    VkBufferImageCopy buffer_image_copy{};
    buffer_image_copy.bufferOffset = 0;
    buffer_image_copy.bufferRowLength = 0;
    buffer_image_copy.bufferImageHeight = 0;

    buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    buffer_image_copy.imageSubresource.mipLevel = 0;
    buffer_image_copy.imageSubresource.baseArrayLayer = 0;
    buffer_image_copy.imageSubresource.layerCount = 1;

    buffer_image_copy.imageOffset = {0, 0, 0};
    buffer_image_copy.imageExtent = {image.description.width, image.description.height, image.description.depth};

    barrier(image_handle, vk::ImageUsage::TransferDst);
    vkCmdCopyBufferToImage(vk_handle, staging_buffer.vk_handle, image.vk_handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &buffer_image_copy);
}

void TransferCommand::upload_image(const Handle<Image>& image_handle, const std::string& path)
{
    int width, height, channels;
    uint8_t* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (data == nullptr)
    {
        throw std::runtime_error("Could not load image from file " + path);
    }
    upload_image(image_handle, data, width * height * 4);
    stbi_image_free(data);
}

void TransferCommand::blit_image(const Handle<Image>& src, const Handle<Image>& dst)
{
    auto& src_image = p_device->images.get(src);
    auto& dst_image = p_device->images.get(dst);

    VkImageBlit blit{};
    blit.srcSubresource.aspectMask = src_image.full_view.range.aspectMask;
    blit.srcSubresource.mipLevel = src_image.full_view.range.baseMipLevel;
    blit.srcSubresource.baseArrayLayer = src_image.full_view.range.baseArrayLayer;
    blit.srcSubresource.layerCount = src_image.full_view.range.layerCount;
    blit.srcOffsets[0] = {.x = 0, .y = 0, .z = 0};
    blit.srcOffsets[1] = {.x = static_cast<int32_t>(src_image.description.width),
                          .y = static_cast<int32_t>(src_image.description.height),
                          .z = static_cast<int32_t>(src_image.description.depth)};
    blit.dstSubresource.aspectMask = dst_image.full_view.range.aspectMask;
    blit.dstSubresource.mipLevel = dst_image.full_view.range.baseMipLevel;
    blit.dstSubresource.baseArrayLayer = dst_image.full_view.range.baseArrayLayer;
    blit.dstSubresource.layerCount = dst_image.full_view.range.layerCount;
    blit.dstOffsets[0] = {.x = 0, .y = 0, .z = 0};
    blit.dstOffsets[1] = {.x = static_cast<int32_t>(dst_image.description.width),
                          .y = static_cast<int32_t>(dst_image.description.height),
                          .z = static_cast<int32_t>(dst_image.description.depth)};

    vkCmdBlitImage(vk_handle, src_image.vk_handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst_image.vk_handle,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_NEAREST);
}
} // namespace vk
