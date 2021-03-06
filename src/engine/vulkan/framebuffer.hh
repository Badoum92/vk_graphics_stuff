#pragma once

#include <vector>
#include <optional>

#include <vulkan/vulkan.h>

#include "handle.hh"

namespace vk
{
struct Image;

struct LoadOp
{
    static LoadOp load()
    {
        LoadOp op{};
        op.vk_loadop = VK_ATTACHMENT_LOAD_OP_LOAD;
        return op;
    }

    static LoadOp dont_care()
    {
        LoadOp op{};
        op.vk_loadop = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        return op;
    }

    static LoadOp clear(float r, float g, float b, float a)
    {
        LoadOp op{};
        op.vk_loadop = VK_ATTACHMENT_LOAD_OP_CLEAR;
        op.clear_value.color.float32[0] = r;
        op.clear_value.color.float32[1] = g;
        op.clear_value.color.float32[2] = b;
        op.clear_value.color.float32[3] = a;
        return op;
    }

    static LoadOp clear(float depth, uint32_t stencil)
    {
        LoadOp op{};
        op.vk_loadop = VK_ATTACHMENT_LOAD_OP_CLEAR;
        op.clear_value.depthStencil = {depth, stencil};
        return op;
    }

    static LoadOp clear_color()
    {
        return clear(0, 0, 0, 1);
    }

    static LoadOp clear_depth()
    {
        return clear(0, 0);
    }

    bool operator==(const LoadOp& other) const
    {
        return vk_loadop == other.vk_loadop;
    }

    VkAttachmentLoadOp vk_loadop = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    VkClearValue clear_value;
};

struct RenderPass
{
    VkRenderPass vk_handle = VK_NULL_HANDLE;
    std::vector<LoadOp> load_ops;
};

struct FrameBufferDescription
{
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t layer_count = 1;
    std::vector<VkFormat> color_formats;
    std::optional<VkFormat> depth_format = std::nullopt;
};

struct FrameBuffer
{
    FrameBufferDescription description;
    VkFramebuffer vk_handle = VK_NULL_HANDLE;

    std::vector<Handle<Image>> color_attachments;
    Handle<Image> depth_attachment;
    std::vector<RenderPass> renderpasses;
};
} // namespace vk
