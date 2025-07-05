#pragma once

#include <stdint.h>

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4201)
#endif

union color
{
    uint32_t u32;
    struct
    {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };
};

static_assert(sizeof(color) == sizeof(uint32_t));

#if defined(_WIN32)
#pragma warning(pop)
#endif

constexpr color COLOR_WHITE = {.r = 255, .g = 255, .b = 255, .a = 255};
constexpr color COLOR_BLACK = {.r = 0, .g = 0, .b = 0, .a = 255};
constexpr color COLOR_X = {.r = 246, .g = 54, .b = 82, .a = 255};
constexpr color COLOR_Y = {.r = 112, .g = 164, .b = 28, .a = 255};
constexpr color COLOR_Z = {.r = 47, .g = 132, .b = 227, .a = 255};
