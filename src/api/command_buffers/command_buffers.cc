#include "command_buffers.hh"

#include "vk_context/vk_context.hh"

void CommandBuffers::create()
{
    handles_.resize(VkContext::framebuffers.size());

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = VkContext::command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = (uint32_t)handles_.size();

    VK_CHECK(vkAllocateCommandBuffers(VkContext::device, &alloc_info, handles_.data()));
}

void CommandBuffers::destroy()
{
    vkFreeCommandBuffers(VkContext::device, VkContext::command_pool, handles_.size(), handles_.data());
}

size_t CommandBuffers::size() const
{
    return handles_.size();
}

void CommandBuffers::record(size_t i)
{
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; // FIXME
    begin_info.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(handles_[i], &begin_info));

    VkRenderPassBeginInfo renderpass_info{};
    renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpass_info.renderPass = VkContext::renderpass;
    renderpass_info.framebuffer = VkContext::framebuffers[i];
    renderpass_info.renderArea.offset = {0, 0};
    renderpass_info.renderArea.extent = VkContext::swapchain.extent();

    VkClearValue clear_color{};
    clear_color.color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    renderpass_info.clearValueCount = 1;
    renderpass_info.pClearValues = &clear_color;

    vkCmdBeginRenderPass(handles_[i], &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(handles_[i], VK_PIPELINE_BIND_POINT_GRAPHICS, VkContext::pipeline);

    vkCmdDraw(handles_[i], 3, 1, 0, 0);

    vkCmdEndRenderPass(handles_[i]);

    VK_CHECK(vkEndCommandBuffer(handles_[i]));
}
