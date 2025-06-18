#pragma once

#include <stdint.h>

namespace vk
{
struct image;
struct sampler;
} // namespace vk

struct texture
{
    vk::image* image;
    vk::sampler* sampler;
    uint32_t binldess_index;
};

texture texture_create(vk::image* _image_handle, vk::sampler* _sampler_handle, uint32_t _bindless_index = UINT32_MAX);
