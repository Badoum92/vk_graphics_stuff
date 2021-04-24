#include "framebuffer.hh"

#include "vk_context/vk_context.hh"

void FrameBuffer::create(const RenderPass& renderpass, const VkImageView& image_view)
{
    VkImageView attachments[] = {image_view};

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = renderpass;
    framebuffer_info.attachmentCount = 1;
    framebuffer_info.pAttachments = attachments;
    framebuffer_info.width = VkContext::swapchain.extent().width;
    framebuffer_info.height = VkContext::swapchain.extent().height;
    framebuffer_info.layers = 1;

    VK_CHECK(vkCreateFramebuffer(VkContext::device, &framebuffer_info, nullptr, &handle_));
}

void FrameBuffer::destroy()
{
    vkDestroyFramebuffer(VkContext::device, handle_, nullptr);
}
