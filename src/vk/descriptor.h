#pragma once

#include <volk.h>

#include "vk/buffer.h"
#include "vk/image.h"
#include "vk/constants.h"

#include "bul/containers/handle.h"
#include "bul/containers/static_vector.h"

namespace vk
{
struct context;

struct descriptor_set
{
    static descriptor_set create(context* context);
    void destroy(context* context);

    uint32_t insert_descriptor(context* context, bul::handle<image> image_handle, bul::handle<sampler> sampler_handle,
                               VkDescriptorType type);
    void remove_descriptor(context* context, uint32_t index);

    VkDescriptorSetLayout layout;
    bul::handle<buffer> buffer_handle;
    VkDeviceSize size;
    VkDeviceSize offset;
    bul::static_vector<uint32_t, max_binless_descriptors> free_descriptors;
};
} // namespace vk
