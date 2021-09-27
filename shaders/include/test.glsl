#ifndef TEST_H
#define TEST_H

struct Vertex
{
    vec4 position;
    vec4 normal;
    vec2 uv_0;
    vec2 uv_1;
};

layout (set = 0, binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 inv_view;
    mat4 proj;
    mat4 inv_proj;
    vec4 camera_pos;
    uvec2 resolution;
} global;

#endif
