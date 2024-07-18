#version 460
#extension GL_EXT_buffer_reference : require
#extension GL_EXT_nonuniform_qualifier : require

layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer uniform_buffer
{
    vec4 colors[3];
    vec2 positions[3];
    vec2 uvs[3];
    mat4 view;
    mat4 proj;
};

layout(std430, push_constant) uniform push_constant
{
    uniform_buffer ubo;
    uint texture_index;
};

layout(set = 0, binding = 0) uniform sampler2D tex2D[];
layout(set = 0, binding = 0) uniform sampler3D tex3D[];
layout(set = 0, binding = 0, rgba8) uniform image2D image2D_rgba8;
layout(set = 0, binding = 0, r8ui) uniform uimage3D image3D_r8;

layout(location = 0) in vec4 in_color;
layout(location = 1) in vec2 in_uv;

layout(location = 0) out vec4 out_color;

void main()
{
    out_color = in_color + texture(tex2D[0], in_uv);
}
