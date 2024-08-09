#include "vk/descriptor.h"

#include "vk/vk_tools.h"
#include "vk/context.h"

namespace vk
{
descriptor_set descriptor_set::create(context* context)
{
    descriptor_set descriptor_set = {};

    VkDescriptorSetLayoutBinding layout_binding = {};
    layout_binding.binding = 0;
    layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    layout_binding.descriptorCount = max_binless_descriptors;
    layout_binding.stageFlags = VK_SHADER_STAGE_ALL;

    VkDescriptorBindingFlags bindling_flags =
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT_EXT | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT_EXT;
    VkDescriptorSetLayoutBindingFlagsCreateInfo binding_flags_info = {};
    binding_flags_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT;
    binding_flags_info.bindingCount = 1;
    binding_flags_info.pBindingFlags = &bindling_flags;

    VkDescriptorSetLayoutCreateInfo layout_info = {};
    layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.pNext = &binding_flags_info;
    layout_info.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_DESCRIPTOR_BUFFER_BIT_EXT;
    layout_info.bindingCount = 1;
    layout_info.pBindings = &layout_binding;

    VK_CHECK(vkCreateDescriptorSetLayout(context->device, &layout_info, nullptr, &descriptor_set.layout));

    vkGetDescriptorSetLayoutSizeEXT(context->device, descriptor_set.layout, &descriptor_set.size);
    descriptor_set.size =
        bul::align(descriptor_set.size, context->descriptor_buffer_properties.descriptorBufferOffsetAlignment);
    vkGetDescriptorSetLayoutBindingOffsetEXT(context->device, descriptor_set.layout, 0, &descriptor_set.offset);

    buffer_description buffer_description = {};
    buffer_description.size = descriptor_set.size;
    buffer_description.usage = descriptor_buffer_usage | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
    buffer_description.memory_usage = VMA_MEMORY_USAGE_AUTO;
    buffer_description.name = "descriptor buffer";
    descriptor_set.buffer_handle = context->create_buffer(buffer_description);

    for (uint32_t i = 0; i < max_binless_descriptors; ++i)
    {
        descriptor_set.free_descriptors.push_back(max_binless_descriptors - i - 1);
    }

    return descriptor_set;
}

void descriptor_set::destroy(context* context)
{
    context->destroy_buffer(buffer_handle);
    vkDestroyDescriptorSetLayout(context->device, layout, nullptr);
}

uint32_t descriptor_set::create_descriptor(context* context, bul::handle<image> image_handle,
                                           bul::handle<sampler> sampler_handle, VkDescriptorType type)
{
    ASSERT(free_descriptors.size > 0);
    uint32_t index = free_descriptors.pop_back();
    update_descriptor(context, index, image_handle, sampler_handle, type);
    return index;
}

uint32_t descriptor_set::create_empty_descriptor(context* context, VkDescriptorType type)
{
    return create_descriptor(context, context->undefined_image_handle, context->default_sampler, type);
}

void descriptor_set::update_descriptor(context* context, uint32_t index, bul::handle<image> image_handle,
                                       bul::handle<sampler> sampler_handle, VkDescriptorType type)
{
    image& image = context->images.get(image_handle);

    VkDescriptorImageInfo descriptor = {};
    descriptor.sampler = sampler_handle ? context->samplers.get(sampler_handle).vk_handle : VK_NULL_HANDLE;
    descriptor.imageView = image.full_view.vk_handle;

    switch (type)
    {
    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
        descriptor.imageLayout = is_depth(image.description.format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
                                                                    : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        break;
    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        descriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        break;
    default:
        ASSERT(!"Unhandled descriptor type");
        break;
    }

    VkDescriptorGetInfoEXT descriptor_info = {};
    descriptor_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_GET_INFO_EXT;
    descriptor_info.pNext = nullptr;
    descriptor_info.type = type;

    switch (type)
    {
    case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER:
        descriptor_info.data.pCombinedImageSampler = &descriptor;
        break;
    case VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT:
        descriptor_info.data.pInputAttachmentImage = &descriptor;
        break;
    case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
        descriptor_info.data.pStorageImage = &descriptor;
        break;
    default:
        ASSERT(!"Unhandled descriptor type");
        break;
    }

    size_t descriptor_size = context->descriptor_buffer_properties.combinedImageSamplerDescriptorSize;
    buffer& buffer = context->buffers.get(buffer_handle);
    vkGetDescriptorEXT(context->device, &descriptor_info, descriptor_size,
                       (uint8_t*)buffer.mapped_data + index * descriptor_size + offset);
}

void descriptor_set::destroy_descriptor(context*, uint32_t index)
{
    free_descriptors.push_back(index);
}
} // namespace vk
