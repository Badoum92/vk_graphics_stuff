#pragma once

#include <vector>

#include <volk.h>

#include "fwd.h"
#include "bul/containers/handle.h"

namespace vk
{
struct ImageDescriptor
{
    bul::Handle<Image> handle;
};

struct BufferDescriptor
{
    bul::Handle<Buffer> handle;
};

struct DynamicDescriptor
{
    bul::Handle<Buffer> handle;
    uint32_t offset = 0;
    uint32_t size = 0;
};

struct DescriptorType
{
    enum class Type : uint16_t
    {
        SampledImage,
        StorageImage,
        StorageBuffer,
        DynamicBuffer
    };

    struct Info
    {
        uint16_t count = 1;
        Type type;
    };

    union
    {
        Info info;
        uint32_t binary;
    };

    static DescriptorType create(Type type, uint16_t count = 1)
    {
        DescriptorType desc_type{{}};
        desc_type.info.count = count;
        desc_type.info.type = type;
        return desc_type;
    }
};

struct Descriptor
{
    union
    {
        ImageDescriptor image;
        BufferDescriptor buffer;
        DynamicDescriptor dynamic;
        uint64_t binary[2];
    };
};

struct DescriptorSet
{
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    std::vector<DescriptorType> descriptor_types;
    size_t layout_hash = 0;

    std::vector<VkDescriptorSet> vk_sets;
    std::vector<size_t> hashes;

    std::vector<Descriptor> descriptors;
    std::vector<uint32_t> dynamic_descriptors;
    std::vector<uint32_t> dynamic_offsets;

    void bind_image(uint32_t binding, const bul::Handle<Image>& image);
    void bind_storage_buffer(uint32_t binding, const bul::Handle<Buffer>& buffer);
    void bind_uniform_buffer(uint32_t binding, const bul::Handle<Buffer>& buffer, uint32_t offset, uint32_t size);
    VkDescriptorSet get_or_create_vk_set();
};
} // namespace vk

namespace std
{
template <>
struct hash<vk::Descriptor>
{
    size_t operator()(const vk::Descriptor& descriptor) const noexcept
    {
        return descriptor.binary[0];
    }
};

template <>
struct hash<vk::DescriptorType>
{
    size_t operator()(const vk::DescriptorType& descriptor_type) const noexcept
    {
        return descriptor_type.binary;
    }
};
} // namespace std
