#include "framebuffer.h"

#include "bul/bul.h"

#include "vk_tools.h"
#include "device.h"

namespace vk
{
RenderPass RenderPass::create(const FrameBufferDescription& description, const std::vector<LoadOp>& load_ops)
{
    ASSERT(description.color_formats.size() + description.depth_format.has_value() == load_ops.size());

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
        attachment_desc.initialLayout = load_ops[i].vk_loadop == VK_ATTACHMENT_LOAD_OP_LOAD
            ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
            : VK_IMAGE_LAYOUT_UNDEFINED;
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
        attachment_desc.initialLayout = load_ops.back().vk_loadop == VK_ATTACHMENT_LOAD_OP_LOAD
            ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
            : VK_IMAGE_LAYOUT_UNDEFINED;
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
    VK_CHECK(vkCreateRenderPass(device::vk_handle, &render_pass_info, nullptr, &vk_renderpass));

    return {vk_renderpass, load_ops};
}

const RenderPass& RenderPass::get_or_create(const bul::Handle<FrameBuffer>& handle, const std::vector<LoadOp>& load_ops)
{
    auto& fb = device::framebuffers.get(handle);
    for (const auto& renderpass : fb.renderpasses)
    {
        if (renderpass.load_ops == load_ops)
        {
            return renderpass;
        }
    }
    fb.renderpasses.push_back(create(fb.description, load_ops));
    return fb.renderpasses.back();
}

void RenderPass::destroy()
{
    vkDestroyRenderPass(device::vk_handle, vk_handle, nullptr);
    vk_handle = VK_NULL_HANDLE;
}

bul::Handle<FrameBuffer> FrameBuffer::create(const FrameBufferDescription& description,
                                             const std::vector<bul::Handle<Image>>& color_attachments,
                                             const bul::Handle<Image>& depth_attachment)
{
    ASSERT(description.color_formats.size() == color_attachments.size());
    ASSERT(description.depth_format.has_value() == depth_attachment.is_valid());

    FrameBuffer framebuffer{};
    framebuffer.description = description;
    framebuffer.color_attachments = color_attachments;
    framebuffer.depth_attachment = depth_attachment;

    uint32_t attachments_count = color_attachments.size() + depth_attachment.is_valid();

    std::vector<VkImageView> attachment_views;
    attachment_views.reserve(attachments_count);
    for (size_t i = 0; i < color_attachments.size(); ++i)
    {
        auto& image = device::images.get(color_attachments[i]);
        attachment_views.push_back(image.full_view.vk_handle);
        ASSERT(image.full_view.format == description.color_formats[i]);
    }
    if (framebuffer.depth_attachment.is_valid())
    {
        auto& image = device::images.get(framebuffer.depth_attachment);
        attachment_views.push_back(image.full_view.vk_handle);
        ASSERT(image.full_view.format == description.depth_format);
    }

    framebuffer.renderpasses.push_back(
        RenderPass::create(description, std::vector<LoadOp>(attachments_count, LoadOp::dont_care())));

    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_info.renderPass = framebuffer.renderpasses[0].vk_handle;
    framebuffer_info.attachmentCount = attachments_count;
    framebuffer_info.pAttachments = attachment_views.data();
    framebuffer_info.width = framebuffer.description.width;
    framebuffer_info.height = framebuffer.description.height;
    framebuffer_info.layers = framebuffer.description.layer_count;

    framebuffer.vk_handle = VK_NULL_HANDLE;
    VK_CHECK(vkCreateFramebuffer(device::vk_handle, &framebuffer_info, nullptr, &framebuffer.vk_handle));

    return device::framebuffers.insert(std::move(framebuffer));
}

void FrameBuffer::destroy(bul::Handle<FrameBuffer> handle)
{
    device::framebuffers.get(handle).destroy();
    device::framebuffers.erase(handle);
}

void FrameBuffer::destroy()
{
    vkDestroyFramebuffer(device::vk_handle, vk_handle, nullptr);
    vk_handle = VK_NULL_HANDLE;
    for (auto& renderpass : renderpasses)
    {
        renderpass.destroy();
    }
    renderpasses.clear();
    color_attachments.clear();
    depth_attachment = bul::Handle<Image>::invalid;
}
} // namespace vk
