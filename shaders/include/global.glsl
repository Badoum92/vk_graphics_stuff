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
    return float(wang_hash(rng)) / 4294967296.0f;
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

vec3 rgb_from_uint(uint u)
{
    return vec3(((u >> 0) & 0xff), ((u >> 8) & 0xff), ((u >> 16) & 0xff)) / 255.0f;
}

vec4 rgba_from_uint(uint u)
{
    return vec4(((u >> 0) & 0xff), ((u >> 8) & 0xff), ((u >> 16) & 0xff), ((u >> 24) & 0xff)) / 255.0f;
}

vec3 turbo_colormap(float x)
{
    const vec4 kRedVec4 = vec4(0.13572138f, 4.61539260f, -42.66032258f, 132.13108234f);
    const vec4 kGreenVec4 = vec4(0.09140261f, 2.19418839f, 4.84296658f, -14.18503333f);
    const vec4 kBlueVec4 = vec4(0.10667330f, 12.64194608f, -60.58204836f, 110.36276771f);
    const vec2 kRedVec2 = vec2(-152.94239396f, 59.28637943f);
    const vec2 kGreenVec2 = vec2(4.27729857f, 2.82956604f);
    const vec2 kBlueVec2 = vec2(-89.90310912f, 27.34824973f);

    x = clamp(x, 0.0f, 1.0f);
    vec4 v4 = vec4(1.0f, x, x * x, x * x * x);
    vec2 v2 = v4.zw * v4.z;
    return vec3(dot(v4, kRedVec4) + dot(v2, kRedVec2), dot(v4, kGreenVec4) + dot(v2, kGreenVec2),
                dot(v4, kBlueVec4) + dot(v2, kBlueVec2));
}

vec3 v3_less_than(vec3 v, float f)
{
    return vec3(v.x < f ? 1.0f : 0.0f, v.y < f ? 1.0f : 0.0f, v.z < f ? 1.0f : 0.0f);
}

vec3 linear_to_srgb(vec3 c)
{
    c = clamp(c, 0.0f, 1.0f);
    return mix(pow(c, vec3(1.0f / 2.4f)) * 1.055f - 0.055f, c * 12.92f, v3_less_than(c, 0.0031308f));
}

vec3 srgb_to_linear(vec3 c)
{
    c = clamp(c, 0.0f, 1.0f);
    return mix(pow(((c + 0.055f) / 1.055f), vec3(2.4f)), c / 12.92f, v3_less_than(c, 0.04045f));
}

vec3 ACES_tone_mapping(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

#endif
