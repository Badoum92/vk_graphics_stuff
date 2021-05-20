#ifndef GLOBAL_H
#define GLOBAL_H

layout (set = 0, binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 inv_view;
    mat4 proj;
    mat4 inv_proj;
    vec3 camera_pos;
    float pad0;
    ivec2 resolution;
    vec2 pad1;
} global;

float max(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

float min(vec3 v)
{
    return min(min(v.x, v.y), v.z);
}

#endif
