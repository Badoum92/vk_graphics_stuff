#ifndef GLOBAL_H
#define GLOBAL_H

layout (set = 0, binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 inv_view;
    mat4 proj;
    mat4 inv_proj;
    vec4 camera_pos;
    uvec2 resolution;
    uint frame_number;
    uint pad0;
} global;

struct Vertex
{
    vec4 position;
    vec4 normal;
    vec2 uv_0;
    vec2 uv_1;
};

const float FLT_P_INF = 1.0f / 0.0f;
const float FLT_N_INF = -1.0f / 0.0f;
const float EPSILON = 0.001f;
const float TWO_PI = 6.2831853f;

bool float_eq(float f1, float f2)
{
    return abs(f1 - f2) < EPSILON;
}

#endif
