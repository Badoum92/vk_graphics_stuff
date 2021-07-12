#include "descriptor_set.hh"

#include <cassert>

#include "hash.hh"
#include "vk_tools.hh"
#include "device.hh"

namespace vk
{
static VkDescriptorType to_vk(DescriptorType::Type type)
{
    switch (type)
    {
    case DescriptorType::Type::SampledImage:
        return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    case DescriptorType::Type::StorageImage:
        return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case DescriptorType::Type::StorageBuffer:
        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case DescriptorType::Type::DynamicBuffer:
        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    }
}

DescriptorSet Device::create_descriptor_set(const std::vector<DescriptorType>& descriptor_types)
{
    DescriptorSet descriptor_set{};

    std::vector<VkDescriptorSetLayoutBinding> bindings(descriptor_types.size());

    for (size_t i = 0; i < bindings.size(); ++i)
    {
        auto& binding = bindings[i];
        auto& descriptor_type = descriptor_types[i];

        binding.binding = i;
        binding.descriptorCount = descriptor_type.info.count;
        binding.descriptorType = to_vk(descriptor_type.info.type);
        binding.stageFlags = VK_SHADER_STAGE_ALL;

        if (descriptor_type.info.type == DescriptorType::Type::DynamicBuffer)
        {
            descriptor_set.dynamic_descriptors.push_back(i);
        }
    }

    Descriptor empty = {{{}}};
    descriptor_set.descriptors.resize(descriptor_types.size(), empty);
    descriptor_set.dynamic_offsets.resize(descriptor_set.dynamic_descriptors.size());

    VkDescriptorSetLayoutCreateInfo layout_info{};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = bindings.size();
    layout_info.pBindings = bindings.data();

    descriptor_set.descriptor_types = descriptor_types;
    descriptor_set.layout_hash = hash_value(descriptor_types);
    VK_CHECK(vkCreateDescriptorSetLayout(vk_handle, &layout_info, nullptr, &descriptor_set.layout));

    return descriptor_set;
}

void DescriptorSet::bind_image(uint32_t binding, const Handle<Image>& image)
{
    assert(descriptor_types[binding].info.type == DescriptorType::Type::SampledImage
           || descriptor_types[binding].info.type == DescriptorType::Type::StorageImage);
    descriptors[binding].image = {image};
}

void DescriptorSet::bind_storage_buffer(uint32_t binding, const Handle<Buffer>& buffer)
{
    assert(descriptor_types[binding].info.type == DescriptorType::Type::StorageBuffer);
    descriptors[binding].buffer = {buffer};
}

void DescriptorSet::bind_uniform_buffer(uint32_t binding, const Handle<Buffer>& buffer, uint32_t offset, uint32_t size)
{
    assert(descriptor_types[binding].info.type == DescriptorType::Type::DynamicBuffer);
    descriptors[binding].dynamic = {buffer, offset, size};

    for (size_t i = 0; i < dynamic_descriptors.size(); ++i)
    {
        if (dynamic_descriptors[i] == binding)
        {
            dynamic_offsets[i] = offset;
            return;
        }
    }
}

VkDescriptorSet DescriptorSet::get_or_create_vk_set(Device& device)
{
    size_t hash = hash_value(descriptors);
    for (size_t i = 0; i < hashes.size(); ++i)
    {
        if (hashes[i] == hash)
        {
            return vk_sets[i];
        }
    }

    VkDescriptorSetAllocateInfo set_info{};
    set_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    set_info.descriptorPool = device.descriptor_pool;
    set_info.pSetLayouts = &layout;
    set_info.descriptorSetCount = 1;

    VkDescriptorSet vk_set = VK_NULL_HANDLE;
    VK_CHECK(vkAllocateDescriptorSets(device.vk_handle, &set_info, &vk_set));
    vk_sets.push_back(vk_set);
    hashes.push_back(hash);

    std::vector<VkWriteDescriptorSet> writes(descriptors.size());
    std::vector<VkDescriptorImageInfo> images_info;
    std::vector<VkDescriptorBufferInfo> buffers_info;
    buffers_info.reserve(descriptors.size());
    images_info.reserve(descriptors.size());

    for (size_t i = 0; i < descriptors.size(); ++i)
    {
        writes[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writes[i].dstSet = vk_set;
        writes[i].dstBinding = i;
        writes[i].descriptorCount = descriptor_types[i].info.count;
        writes[i].descriptorType = to_vk(descriptor_types[i].info.type);

        if (descriptor_types[i].info.type == DescriptorType::Type::SampledImage)
        {
            auto& image = device.images.get(descriptors[i].image.handle);
            images_info.push_back({
                .sampler = device.samplers[0],
                .imageView = image.full_view.vk_handle,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            });
            writes[i].pImageInfo = &images_info.back();
        }
        else if (descriptor_types[i].info.type == DescriptorType::Type::StorageImage)
        {
            auto& image = device.images.get(descriptors[i].image.handle);
            images_info.push_back({
                .sampler = VK_NULL_HANDLE,
                .imageView = image.full_view.vk_handle,
                .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
            });
            writes[i].pImageInfo = &images_info.back();
        }
        else if (descriptor_types[i].info.type == DescriptorType::Type::DynamicBuffer)
        {
            auto& dynamic_descriptor = descriptors[i].dynamic;
            auto& buffer = device.buffers.get(dynamic_descriptor.handle);
            buffers_info.push_back({
                .buffer = buffer.vk_handle,
                .offset = 0,
                .range = dynamic_descriptor.size,
            });
            writes[i].pBufferInfo = &buffers_info.back();
        }
        else if (descriptor_types[i].info.type == DescriptorType::Type::StorageBuffer)
        {
            auto& buffer = device.buffers.get(descriptors[i].buffer.handle);
            buffers_info.push_back({
                .buffer = buffer.vk_handle,
                .offset = 0,
                .range = buffer.description.size,
            });
            writes[i].pBufferInfo = &buffers_info.back();
        }
    }

    vkUpdateDescriptorSets(device.vk_handle, writes.size(), writes.data(), 0, nullptr);

    return vk_set;
}

void Device::destroy_descriptor_set(DescriptorSet& descriptor_set)
{
    vkDestroyDescriptorSetLayout(vk_handle, descriptor_set.layout, nullptr);
    descriptor_set.layout = VK_NULL_HANDLE;
}

}; // namespace vk
