#include "texture.h"

texture texture_create(vk::image* _image_handle, vk::sampler* _sampler_handle, uint32_t _bindless_index)
{
    return texture{_image_handle, _sampler_handle, _bindless_index};
}