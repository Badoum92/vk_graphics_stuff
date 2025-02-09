#pragma once

#include <volk.h>

#include "vk/vk_buffer.h"
#include "vk/vk_image.h"
#include "vk/vk_constants.h"

#include "bul/containers/handle.h"
#include "bul/containers/static_vector.h"

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
    VkDeviceSize size;
    VkDeviceSize offset;
    bul::static_vector<uint32_t, max_binless_descriptors> free_descriptors;
};
} // namespace vk
