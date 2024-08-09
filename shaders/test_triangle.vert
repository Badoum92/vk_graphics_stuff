#version 460
#extension GL_EXT_buffer_reference : require

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

layout(location = 0) out vec4 out_normal;
layout(location = 1) out vec2 out_uv;

void main()
{
    out_normal = vb.vertices[gl_VertexIndex].normal;
    out_uv = vb.vertices[gl_VertexIndex].uv;
    gl_Position = ubo.mvp * vb.vertices[gl_VertexIndex].position;
}
