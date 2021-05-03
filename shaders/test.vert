#include "global.glsl"

layout(location = 0) out vec3 out_color;
layout(location = 1) out vec2 out_tex_coord;

struct Vertex
{
    vec2 pos;
    vec2 tex_coord;
};

layout(set = 1, binding = 0) buffer VertexBuffer
{
    Vertex vertices[];
};

void main()
{
    gl_Position = vec4(vertices[gl_VertexIndex].pos, 0.0, 1.0);
    out_color = global.colors[gl_VertexIndex].xyz;
    out_tex_coord = vertices[gl_VertexIndex].tex_coord;
}
