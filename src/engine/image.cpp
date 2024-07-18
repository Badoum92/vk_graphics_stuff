#include "image.h"

#include <stb/stb_image.h>

#include "bul/bul.h"

image image::from_file(const char* path)
{
    image image;
    int x, y, channels;
    image.bytes = stbi_load(path, &x, &y, &channels, 4);
    ASSERT(image.bytes != nullptr);
    image.width = (uint32_t)x;
    image.height = (uint32_t)y;
    return image;
}

void image::destroy()
{
    stbi_image_free(bytes);
}

uint32_t image::size_bytes()
{
    return width * height * 4;
}