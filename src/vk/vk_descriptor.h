#pragma once

#include <volk.h>

#include "vk/vk_constants.h"

namespace vk
{
struct context;
struct buffer;
struct image;
struct sampler;

struct descriptor_set
{
    static descriptor_set create(context* context, VkDescriptorType type);
    void destroy(context* context);

    uint32_t create_texture_descriptor(context* context, image* image, sampler* sampler);
    void update_texture_descriptor(context* context, uint32_t index, image* image, sampler* sampler);

    uint32_t create_image_descriptor(context* context, image* image);
    void update_image_descriptor(context* context, uint32_t index, image* image);

    void destroy_descriptor(uint32_t index);

    VkDescriptorSetLayout layout;
    buffer* buffer;
    uint32_t size;
    uint32_t offset;
    uint32_t free_descriptors[max_binless_descriptors];
    uint32_t num_free_descriptors;
};
} // namespace vk
