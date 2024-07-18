#version 460
#extension GL_EXT_buffer_reference : require

struct vertex
{
    vec4 position;
    vec4 color;
    vec2 uv;
    vec2 _padding;
};

layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer vertex_buffer
{
    vertex vertices[];
};

layout(std430, buffer_reference, buffer_reference_align = 8) readonly buffer uniform_buffer
{
    mat4 view_proj;
};

layout(std430, push_constant) uniform push_constant
{
    vertex_buffer vb;
    uniform_buffer ubo;
};

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec2 out_uv;

void main()
{
    out_color = vb.vertices[gl_VertexIndex].color;
    out_uv = vb.vertices[gl_VertexIndex].uv;
    gl_Position = ubo.view_proj * vb.vertices[gl_VertexIndex].position;
}
