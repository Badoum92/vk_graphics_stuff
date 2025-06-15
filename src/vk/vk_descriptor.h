#pragma once

#include <volk.h>

#include "vk/vk_buffer.h"
#include "vk/vk_image.h"
#include "vk/vk_constants.h"

#include "core/containers/handle.h"

namespace vk
{
struct context;

struct descriptor_set
{
    static descriptor_set create(context* context, VkDescriptorType type);
    void destroy(context* context);

    uint32_t create_texture_descriptor(context* context, bul::handle<image> image_handle,
                                       bul::handle<sampler> sampler_handle);
    void update_texture_descriptor(context* context, uint32_t index, bul::handle<image> image_handle,
                                   bul::handle<sampler> sampler_handle);

    uint32_t create_image_descriptor(context* context, bul::handle<image> image_handle);
    void update_image_descriptor(context* context, uint32_t index, bul::handle<image> image_handle);

    void destroy_descriptor(uint32_t index);

    VkDescriptorSetLayout layout;
    bul::handle<buffer> buffer_handle;
    uint32_t size;
    uint32_t offset;
    uint32_t free_descriptors[max_binless_descriptors];
    uint32_t num_free_descriptors;
};
} // namespace vk
