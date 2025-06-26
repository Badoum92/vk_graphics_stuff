#ifndef GLOBAL_H
#define GLOBAL_H

#version 460

#extension GL_EXT_buffer_reference : require
#extension GL_EXT_nonuniform_qualifier : require

layout(set = 0, binding = 0) uniform sampler2D tex2D[];
layout(set = 0, binding = 0) uniform sampler3D tex3D[];
layout(set = 1, binding = 0, rgba8) uniform image2D image2D_rgba8[];
layout(set = 1, binding = 0, r8ui) uniform uimage3D image3D_r8[];

#define BUFFER(NAME)        layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer NAME
#define PUSH_CONSTANT(NAME) layout(std430, push_constant) uniform NAME
#define INPUT(N, NAME)      layout(location = N) in NAME
#define OUTPUT(N, NAME)     layout(location = N) out NAME

#define VERTEX_INDEX        gl_VertexIndex
#define GOBAL_INVOCATION_ID gl_GlobalInvocationID

const float FLT_P_INF = 1.0f / 0.0f;
const float FLT_N_INF = -1.0f / 0.0f;
const float EPSILON = 0.0001f;
const float TWO_PI = 6.2831853f;

uint init_rng(uvec2 pixel, uint frame_nb)
{
    return uint(uint(pixel.x) * uint(1973) + uint(pixel.y) * uint(9277) + uint(frame_nb) * uint(26699)) | uint(1);
}

uint wang_hash(inout uint n)
{
    n = uint(n ^ uint(61)) ^ uint(n >> uint(16));
    n *= uint(9);
    n = n ^ (n >> 4);
    n *= uint(0x27d4eb2d);
    n = n ^ (n >> 15);
    return n;
}

float random_float_01(inout uint rng)
{
    return float(wang_hash(rng)) / 4294967296.0;
}

vec3 random_unit_vector(inout uint rng)
{
    float z = random_float_01(rng) * 2.0f - 1.0f;
    float a = random_float_01(rng) * TWO_PI;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}

uint min_comp3(vec3 v)
{
    return uint((v.y < v.z) && (v.y < v.x)) + uint((v.z < v.y) && (v.z < v.x)) * 2;
}

#endif
