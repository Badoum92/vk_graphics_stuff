#pragma once

#include <stdint.h>

struct color
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct image
{
    static image from_file(const char* path);
    void destroy();

    uint32_t size_bytes();

    union
    {
        color* colors;
        uint8_t* bytes;
    };
    uint32_t width;
    uint32_t height;
};