#pragma once

#include "bul/containers/handle.h"

namespace vk
{
struct image;
struct sampler;
}

struct texture
{
    static texture create(bul::handle<vk::image> _image_handle, bul::handle<vk::sampler> _sampler_handle,
                          uint32_t _bindless_index = UINT32_MAX);

    bul::handle<vk::image> image_handle;
    bul::handle<vk::sampler> sampler_handle;
    uint32_t binldess_index;
};