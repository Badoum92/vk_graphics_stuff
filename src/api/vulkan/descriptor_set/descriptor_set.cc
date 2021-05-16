#include "descriptor_set.hh"

#include <cassert>

#include "vk_context/vk_context.hh"

DescriptorSet DescriptorSet::global_set;
bool DescriptorSet::global_set_initialized = false;;

static VkDescriptorType buffer_to_descriptor_type(const Buffer& buffer)
{
    auto usage = buffer.buffer_usage();
    if (usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    if (usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
    assert(false && "Invalid buffer usage");
    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

void DescriptorSet::create(VkDescriptorSetLayout vk_layout, const Layout& layout)
{
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = VkContext::descriptor_pool;
    alloc_info.descriptorSetCount = 1;
    alloc_info.pSetLayouts = &vk_layout;

    VK_CHECK(vkAllocateDescriptorSets(VkContext::device, &alloc_info, &handle_));

    uint32_t i = 0;
    for (const auto& layout_binding : layout)
    {
        if (layout_binding.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC
            || layout_binding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)
        {
            dynamic_offset_indices_[layout_binding.binding] = i++;
            dynamic_offsets_.push_back(0);
        }
    }
}

void DescriptorSet::destroy()
{
    writes_.clear();
    buffer_infos_.clear();
    image_infos_.clear();
    dynamic_offset_indices_.clear();
    dynamic_offsets_.clear();
}

void DescriptorSet::bind_buffer(uint32_t binding, const Buffer& buffer, size_t range, size_t offset)
{
    buffer_infos_.emplace_back();
    auto& buffer_info = buffer_infos_.back();
    buffer_info.buffer = buffer;
    buffer_info.offset = offset;
    buffer_info.range = range > 0 ? range : buffer.size();

    writes_.emplace_back();
    auto& write = writes_.back();
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = handle_;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = buffer_to_descriptor_type(buffer);
    write.descriptorCount = 1;
    write.pBufferInfo = &buffer_info;
    write.pImageInfo = nullptr;
    write.pTexelBufferView = nullptr;
}

void DescriptorSet::bind_image_sampler(uint32_t binding, const Image& image, const Sampler& sampler)
{
    image_infos_.emplace_back();
    auto& image_info = image_infos_.back();
    image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    image_info.imageView = image.view();
    image_info.sampler = sampler;

    writes_.emplace_back();
    auto& write = writes_.back();
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = handle_;
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pBufferInfo = nullptr;
    write.pImageInfo = &image_info;
    write.pTexelBufferView = nullptr;
}

void DescriptorSet::update_dynamic_offset(uint32_t binding, size_t offset)
{
    dynamic_offsets_[dynamic_offset_indices_[binding]] = offset;
}

void DescriptorSet::update()
{
    if (writes_.empty())
        return;
    vkUpdateDescriptorSets(VkContext::device, writes_.size(), writes_.data(), 0, nullptr);
    writes_.clear();
    buffer_infos_.clear();
}

void DescriptorSet::bind(VkPipelineBindPoint bind_point, VkPipelineLayout layout)
{
    vkCmdBindDescriptorSets(FrameData::current().cmd, bind_point, layout, 0, 1, &handle_,
                            dynamic_offsets_.size(), dynamic_offsets_.data());
}

void DescriptorSet::create_global_set(VkDescriptorSetLayout vk_layout, const Layout& layout)
{
    if (global_set_initialized)
        return;
    global_set.create(vk_layout, layout);
    global_set_initialized = true;
}

void DescriptorSet::destroy_global_set()
{
    if (!global_set_initialized)
        return;
    global_set.destroy();
    global_set_initialized = false;
}

const std::vector<uint32_t>& DescriptorSet::dynamic_offsets() const
{
    return dynamic_offsets_;
}
