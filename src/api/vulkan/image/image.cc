#include "image.hh"

#include <stb/stb_image.h>

#include "vk_context/vk_context.hh"
#include "buffer/buffer.hh"

Image::~Image()
{
    destroy();
}

void Image::create(uint32_t width, uint32_t height, uint32_t depth, VkFormat format, VkImageUsageFlags usage,
                   VkSampleCountFlagBits sample_count)
{
    format_ = format;
    usage_ = usage;
    sample_count_ = sample_count;
    type_ = depth == 1 ? VK_IMAGE_TYPE_2D : VK_IMAGE_TYPE_3D;
    layout_ = VK_IMAGE_LAYOUT_UNDEFINED;

    width_ = width;
    height_ = height;
    depth_ = depth;

    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = type_;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = depth;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.format = format_;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.initialLayout = layout_;
    image_info.usage = usage_;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = sample_count;
    image_info.flags = 0;

    VmaAllocationCreateInfo alloc_info{};
    alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VK_CHECK(vmaCreateImage(VkContext::allocator, &image_info, &alloc_info, &handle_, &alloc_, nullptr));

    VkImageAspectFlags aspect_flags =
        has_depth_component(format) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;

    view_.create(handle_, type_, format_, aspect_flags);
}

void Image::destroy()
{
    view_.destroy();

    vmaDestroyImage(VkContext::allocator, handle_, alloc_);
}

void Image::create(const std::filesystem::path& path)
{
    int width, height, channels;
    uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (data == nullptr)
    {
        throw std::runtime_error("Could not load image from file " + path.string());
    }

    create(width, height);
    fill(data, width * height * 4);
    stbi_image_free(data);

    transition_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Image::create(uint32_t width, uint32_t height, uint32_t depth)
{
    create(width, height, depth, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
           VK_SAMPLE_COUNT_1_BIT);
}

void Image::create_depth_attachment(uint32_t width, uint32_t height)
{
    create(width, height, 1, get_depth_format(),
           VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_SAMPLE_COUNT_1_BIT);
}

void Image::fill(const void* data, size_t size)
{
    Buffer staging;
    staging.create_staging(data, size);

    transition_layout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    vk_execute_once([&](VkCommandBuffer cmd) {
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width_, height_, depth_};

        vkCmdCopyBufferToImage(cmd, staging, handle_, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    });
}

void Image::transition_layout(VkImageLayout new_layout)
{
    vk_execute_once([&](VkCommandBuffer cmd) {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = layout_;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = handle_;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags src_stage;
        VkPipelineStageFlags dst_stage;

        if (layout_ == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if (layout_ == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
                 && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::runtime_error("Unsupported layout transition");
        }

        vkCmdPipelineBarrier(cmd, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        layout_ = new_layout;
    });
}

const ImageView& Image::view() const
{
    return view_;
}

VkFormat Image::format() const
{
    return format_;
}

VkImageType Image::type() const
{
    return type_;
}

VkImageLayout Image::layout() const
{
    return layout_;
}
