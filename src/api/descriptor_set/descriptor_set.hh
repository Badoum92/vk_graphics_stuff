#pragma once

#include <vulkan/vulkan.h>
#include <map>
#include <vector>

#include "buffers/buffer.hh"

class DescriptorSet
{
public:
    using Layout = std::vector<VkDescriptorSetLayoutBinding>;

    void create(VkDescriptorSetLayout vk_layout, const Layout& layout);
    void destroy();

    void bind_buffer(uint32_t binding, const Buffer& buffer, size_t range = 0, size_t offset = 0);
    // void bind_image(uint32_t binding, const Image& image);
    void update_dynamic_offset(uint32_t binding, size_t offset);
    void update();

    void bind(VkPipelineBindPoint bind_point, VkPipelineLayout layout);

    static void create_global_set(VkDescriptorSetLayout vk_layout, const Layout& layout);
    static void destroy_global_set();
    static DescriptorSet global_set;

private:
    VkDescriptorSet handle_;
    std::vector<VkWriteDescriptorSet> writes_;
    std::vector<VkDescriptorBufferInfo> buffer_infos_;
    std::map<uint32_t, size_t> dynamic_offset_indices_;
    std::vector<uint32_t> dynamic_offsets_;

    static bool global_set_initialized;
};
