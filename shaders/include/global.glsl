#ifndef GLOBAL_H
#define GLOBAL_H

layout (set = 0, binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 inv_view;
    mat4 proj;
    mat4 inv_proj;
} global;

#endif
