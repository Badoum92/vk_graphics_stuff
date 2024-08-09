#version 460
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_nonuniform_qualifier : require

struct vertex
{
    vec4 position;
    vec4 normal;
    vec2 uv;
    vec2 _padding;
};

layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer vertex_buffer
{
    vertex vertices[];
};

layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer uniform_buffer
{
    mat4 mvp;
};

layout(std430, push_constant) uniform push_constant
{
    vertex_buffer vb;
    uniform_buffer ubo;
};

layout(set = 0, binding = 0) uniform sampler2D tex2D[];
layout(set = 0, binding = 0) uniform sampler3D tex3D[];
layout(set = 0, binding = 0, rgba8) uniform image2D image2D_rgba8;
layout(set = 0, binding = 0, r8ui) uniform uimage3D image3D_r8;

layout(location = 0) in vec4 in_normal;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

void main()
{
    // out_color = vec4(vec3(gl_FragCoord.z), 1);
    out_color = (in_normal + vec4(1.0f)) * 0.5f;
    // out_color = vec4(in_uv, 0, 1);
    // out_color = texture(tex2D[0], in_uv);
}
