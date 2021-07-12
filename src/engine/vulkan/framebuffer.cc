#include "framebuffer.hh"

#include "vk_tools.hh"
#include "device.hh"

namespace vk
{
RenderPass Device::create_renderpass(const FrameBufferDescription& description, const std::vector<LoadOp> load_ops)
{
    assert(description.color_formats.size() + description.depth_format.has_value() == load_ops.size());

    std::vector<VkAttachmentDescription> attachments_descs;
    std::vector<VkAttachmentReference> attachment_refs;

    for (size_t i = 0; i < description.color_formats.size(); ++i)
    {
        VkAttachmentDescription attachment_desc{};
        attachment_desc.format = description.color_formats[i];
        attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_desc.loadOp = load_ops[i].vk_loadop;
        attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_desc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference attachment_ref{};
        attachment_ref.attachment = attachments_descs.size();
        attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        attachments_descs.push_back(attachment_desc);
        attachment_refs.push_back(attachment_ref);
    }

    if (description.depth_format.has_value())
    {
        VkAttachmentDescription attachment_desc{};
        attachment_desc.format = description.depth_format.value();
        attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_desc.loadOp = load_ops.back().vk_loadop;
        attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference attachment_ref{};
        attachment_ref.attachment = attachments_descs.size();
        attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        attachments_descs.push_back(attachment_desc);
        attachment_refs.push_back(attachment_ref);
    }

    VkSubpassDescription subpass{};
    subpass.flags = 0;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = description.color_formats.size();
    subpass.pColorAttachments = attachment_refs.data();
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment =
        description.depth_format.has_value() ? attachment_refs.data() + description.color_formats.size() : nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = attachments_descs.size();
    render_pass_info.pAttachments = attachments_descs.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 0;
    render_pass_info.pDependencies = nullptr;

    VkRenderPass vk_renderpass = VK_NULL_HANDLE;
    VK_CHECK(vkCreateRenderPass(vk_handle, &render_pass_info, nullptr, &vk_renderpass));

    return {vk_renderpass, load_ops};
}

Handle<FrameBuffer> Device::create_framebuffer(const FrameBufferDescription& description,
                                               const std::vector<Handle<Image>>& color_attachments,
                                               Handle<Image> depth_attachment, const std::vector<LoadOp> load_ops)
{
    assert(description.color_formats.size() == color_attachments.size());
    assert(description.depth_format.has_value() == depth_attachment.is_valid());

    FrameBuffer framebuffer{};
    framebuffer.description = description;
    framebuffer.color_attachments = color_attachments;
    framebuffer.depth_attachment = depth_attachment;

    uint32_t attachments_count = color_attachments.size() + depth_attachment.is_valid();

    std::vector<VkImageView> attachment_views;
    attachment_views.reserve(attachments_count);
    for(size_t i = 0; i < color_attachments.size(); ++i)
    {
        auto& image = images.get(color_attachments[i]);
        attachment_views.push_back(image.full_view.vk_handle);
        assert(image.full_view.format == description.color_formats[i]);
    }
    if (framebuffer.depth_attachment.is_valid())
    {
        auto& image = images.get(framebuffer.depth_attachment);
        attachment_views.push_back(image.full_view.vk_handle);
        assert(image.full_view.format == description.depth_format);
    }

    framebuffer.renderpass = create_renderpass(description, load_ops);

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = framebuffer.renderpass.vk_handle;
    framebuffer_info.attachmentCount = attachments_count;
    framebuffer_info.pAttachments = attachment_views.data();
    framebuffer_info.width = framebuffer.description.width;
    framebuffer_info.height = framebuffer.description.height;
    framebuffer_info.layers = framebuffer.description.layer_count;

    framebuffer.vk_handle = VK_NULL_HANDLE;
    VK_CHECK(vkCreateFramebuffer(vk_handle, &framebuffer_info, nullptr, &framebuffer.vk_handle));

    return framebuffers.insert(framebuffer);
}

void Device::destroy_framebuffer(const Handle<FrameBuffer>& handle)
{
    if (!handle.is_valid())
    {
        return;
    }

    FrameBuffer& framebuffer = framebuffers.get(handle);
    vkDestroyFramebuffer(vk_handle, framebuffer.vk_handle, nullptr);
    framebuffer.vk_handle = VK_NULL_HANDLE;
    vkDestroyRenderPass(vk_handle, framebuffer.renderpass.vk_handle, nullptr);
    framebuffer.color_attachments.clear();
    framebuffer.depth_attachment = Handle<Image>::invalid();
    framebuffers.remove(handle);
}
} // namespace vk
