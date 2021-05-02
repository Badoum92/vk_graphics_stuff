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

void CommandBuffer::record(size_t image_index)
{
    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;

    VK_CHECK(vkBeginCommandBuffer(handle_, &begin_info));

    VkClearValue clear_color{};
    clear_color.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

    VkRenderPassBeginInfo renderpass_info{};
    renderpass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderpass_info.renderPass = VkContext::renderpass;
    renderpass_info.framebuffer = VkContext::framebuffers[image_index];
    renderpass_info.renderArea.offset = {0, 0};
    renderpass_info.renderArea.extent = VkContext::swapchain.extent();
    renderpass_info.clearValueCount = 1;
    renderpass_info.pClearValues = &clear_color;

    size_t offset =
        VkContext::global_uniform_buffer.push<glm::mat4>({{1, 1, 0, 0}, {0, 1, 1, 0}, {1, 0, 1, 0}, {1, 1, 1, 0}});
    DescriptorSet::global_set.update_dynamic_offset(0, offset);

    vkCmdBeginRenderPass(handle_, &renderpass_info, VK_SUBPASS_CONTENTS_INLINE);

    VkContext::pipeline.bind();
    VkContext::pipeline.bind_set(0);

    VkContext::vertex_buffer.bind_vertex(handle_, 0);
    VkContext::index_buffer.bind_index(handle_, 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(handle_, VkContext::index_buffer.count<uint16_t>(), 1, 0, 0, 0);

    vkCmdEndRenderPass(handle_);

    VK_CHECK(vkEndCommandBuffer(handle_));
}
