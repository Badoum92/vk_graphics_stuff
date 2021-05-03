#pragma once

#include <vulkan/vulkan.h>
#include <map>
#include <vector>

#include "buffer/buffer.hh"
#include "image/image.hh"
#include "sampler/sampler.hh"

class DescriptorSet
{
public:
    using Layout = std::vector<VkDescriptorSetLayoutBinding>;

    void create(VkDescriptorSetLayout vk_layout, const Layout& layout);
    void destroy();

    void bind_buffer(uint32_t binding, const Buffer& buffer, size_t range = 0, size_t offset = 0);
    void bind_image_sampler(uint32_t binding, const Image& image, const Sampler& sampler);
    void update_dynamic_offset(uint32_t binding, size_t offset);
    void update();

    void bind(VkPipelineBindPoint bind_point, VkPipelineLayout layout);

    const std::vector<uint32_t>& dynamic_offsets() const;

    static void create_global_set(VkDescriptorSetLayout vk_layout, const Layout& layout);
    static void destroy_global_set();
    static DescriptorSet global_set;

    inline const VkDescriptorSet& handle() const
    {
        return handle_;
    }

    inline operator const VkDescriptorSet&() const
    {
        return handle_;
    }

private:
    VkDescriptorSet handle_;
    std::vector<VkWriteDescriptorSet> writes_;
    std::vector<VkDescriptorBufferInfo> buffer_infos_;
    std::vector<VkDescriptorImageInfo> image_infos_;
    std::map<uint32_t, size_t> dynamic_offset_indices_;
    std::vector<uint32_t> dynamic_offsets_;

    static bool global_set_initialized;
};
