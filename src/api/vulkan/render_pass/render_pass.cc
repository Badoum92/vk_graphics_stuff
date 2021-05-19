#include "render_pass.hh"

#include <cassert>

#include "vk_context/vk_context.hh"

static VkAttachmentDescription gen_color_attachment(bool present)
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = VkContext::swapchain.format();
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    if (present)
    {
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    }
    return color_attachment;
}

static VkAttachmentDescription gen_depth_attachment()
{
    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = get_depth_format();
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    return depth_attachment;
}

void RenderPass::create(uint32_t colors, bool depth, bool present)
{
    assert(colors > 0);

    colors_ = colors;
    depth_ = depth;

    std::vector<VkAttachmentDescription> attachments(colors + depth);
    std::vector<VkAttachmentReference> attachment_refs(colors + depth);

    for (uint32_t i = 0; i < colors; ++i)
    {
        attachments[i] = gen_color_attachment(present);
        attachment_refs[i].attachment = i;
        attachment_refs[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    if (depth)
    {
        attachments.back() = gen_depth_attachment();
        attachment_refs.back().attachment = attachments.size() - 1;
        attachment_refs.back().layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = colors;
    subpass.pColorAttachments = attachment_refs.data();
    subpass.pDepthStencilAttachment = depth ? attachment_refs.data() + colors : nullptr;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    if (depth)
    {
        dependency.srcStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        // dependency.srcAccessMask |= 0;
        dependency.dstStageMask |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = attachments.size();
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    VK_CHECK(vkCreateRenderPass(VkContext::device, &render_pass_info, nullptr, &handle_));
}

void RenderPass::destroy()
{
    vkDestroyRenderPass(VkContext::device, handle_, nullptr);
}

uint32_t RenderPass::colors() const
{
    return colors_;
}

bool RenderPass::depth() const
{
    return depth_;
}
