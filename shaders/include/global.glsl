#ifndef GLOBAL_H
#define GLOBAL_H

layout(set = 0, binding = 0) uniform GlobalUniform
{
    mat4 view;
    mat4 inv_view;
    mat4 proj;
    mat4 inv_proj;
    vec4 camera_pos;
    uvec2 resolution;
    float exposure;
    uint frame_number;
}
global;

struct Vertex
{
    vec4 position;
    vec4 normal;
    vec2 uv_0;
    vec2 uv_1;
};

const float FLT_P_INF = 1.0f / 0.0f;
const float FLT_N_INF = -1.0f / 0.0f;
const float EPSILON = 0.01f;
const float TWO_PI = 6.2831853f;

uint init_rng(ivec2 pixel, uint frame_nb)
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

float random_float_01(inout uint rng_state)
{
    return float(wang_hash(rng_state)) / 4294967296.0;
}

vec3 random_unit_vector(inout uint rng_state)
{
    float z = random_float_01(rng_state) * 2.0f - 1.0f;
    float a = random_float_01(rng_state) * TWO_PI;
    float r = sqrt(1.0f - z * z);
    float x = r * cos(a);
    float y = r * sin(a);
    return vec3(x, y, z);
}

float linear_to_srgb(float color)
{
    if (color < 0.0031308f)
        return color * 12.92f;
    else
        return 1.055f * pow(color, 1.0f / 2.4f) - 0.055f;
}

vec3 linear_to_srgb(vec3 color)
{
    return vec3(linear_to_srgb(color.x), linear_to_srgb(color.y), linear_to_srgb(color.z));
}

float srgb_to_linear(float color)
{
    if (color < 0.04045f)
        return color / 12.92f;
    else
        return float(pow((color + 0.055f) / 1.055f, 2.4f));
}

vec3 srgb_to_linear(vec3 color)
{
    return vec3(srgb_to_linear(color.x), srgb_to_linear(color.y), srgb_to_linear(color.z));
}

vec3 ACES_tone_mapping(vec3 v)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((v * (a * v + b)) / (v * (c * v + d) + e), 0.0f, 1.0f);
}

bool float_eq(float f1, float f2)
{
    return abs(f1 - f2) < EPSILON;
}

float max_component(vec3 v)
{
    return max(v.x, max(v.y, v.z));
}

float min_component(vec3 v)
{
    return min(v.x, min(v.y, v.z));
}

float luminance(vec3 rgb)
{
    return dot(rgb, vec3(0.2126f, 0.7152f, 0.0722f));
}

#endif
