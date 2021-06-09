#include "command_buffer.hh"

#include <glm/glm.hpp>

#include "vk_context/vk_context.hh"

void CommandBuffer::create(VkCommandPool pool)
{
    pool_ = pool;

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = 1;

    VK_CHECK(vkAllocateCommandBuffers(VkContext::device, &alloc_info, &handle_));
}

void CommandBuffer::destroy()
{
    vkFreeCommandBuffers(VkContext::device, pool_, 1, &handle_);
}

void CommandBuffer::begin()
{
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;
    VK_CHECK(vkBeginCommandBuffer(handle_, &begin_info));
}

void CommandBuffer::end()
{
    VK_CHECK(vkEndCommandBuffer(handle_));
}

void CommandBuffer::begin_renderpass(const RenderPass& renderpass, const FrameBuffer& framebuffer)
{
    std::vector<VkClearValue> clear_values(renderpass.colors() + renderpass.depth());

    for (uint32_t i = 0; i < renderpass.colors(); ++i)
    {
        clear_values[i].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    }

    if (renderpass.depth())
    {
        clear_values.back().depthStencil = {0.0f, 0};
    }

    VkRenderPassBeginInfo renderpass_info{};
    renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpass_info.renderPass = renderpass;
    renderpass_info.framebuffer = framebuffer;
    renderpass_info.renderArea.offset = {0, 0};
    renderpass_info.renderArea.extent = VkContext::swapchain.extent();
    renderpass_info.clearValueCount = clear_values.size();
    renderpass_info.pClearValues = clear_values.data();

    vkCmdBeginRenderPass(handle_, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void CommandBuffer::end_renderpass()
{
    vkCmdEndRenderPass(handle_);
}

void CommandBuffer::draw(uint32_t vertex_count)
{
    vkCmdDraw(handle_, vertex_count, 1, 0, 0);
}

void CommandBuffer::draw_indexed(uint32_t index_count, uint32_t index_offset, uint32_t instance_count)
{
    vkCmdDrawIndexed(handle_, index_count, instance_count, index_offset, 0, 0);
}

void CommandBuffer::bind_vertex_buffer(const Buffer& buffer, size_t offset)
{
    vkCmdBindVertexBuffers(handle_, 0, 1, &buffer.handle(), &offset);
}

void CommandBuffer::bind_index_buffer(const Buffer& buffer, size_t offset, VkIndexType index_type)
{
    vkCmdBindIndexBuffer(handle_, buffer, offset, index_type);
}

void CommandBuffer::bind_pipeline(const Pipeline& pipeline)
{
    vkCmdBindPipeline(handle_, pipeline.bind_point(), pipeline);
}

void CommandBuffer::bind_descriptor_set(const Pipeline& pipeline, uint32_t n)
{
    const auto& set = pipeline.descriptor_set(n);
    vkCmdBindDescriptorSets(handle_, pipeline.bind_point(), pipeline.layout(), n, 1, &set.handle(),
                            set.dynamic_offsets().size(), set.dynamic_offsets().data());
}

void CommandBuffer::set_viewport(const VkViewport& viewport)
{
    vkCmdSetViewport(handle_, 0, 1, &viewport);
}

void CommandBuffer::set_scissor(const VkRect2D& scissor)
{
    vkCmdSetScissor(handle_, 0, 1, &scissor);
}
