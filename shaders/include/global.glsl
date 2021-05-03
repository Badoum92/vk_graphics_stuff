#ifndef GLOBAL_H
#define GLOBAL_H

layout (set = 0, binding = 0) uniform GlobalUniform
{
    vec4 colors[4];
    // vec4 color_0;
    // vec4 color_1;
    // vec4 color_2;
    // vec4 color_3;
} global;

#endif
