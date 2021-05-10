#include "global.glsl"

layout(location = 0) out vec2 out_tex_coord;

struct Vertex
{
    vec3 pos;
    float _pad0;
    vec2 tex_coord;
    vec2 _pad1;
};

layout(set = 1, binding = 0) buffer VertexBuffer
{
    Vertex vertices[];
};

void main()
{
    gl_Position = global.proj * global.view * vec4(vertices[gl_VertexIndex].pos, 1.0);
    out_tex_coord = vertices[gl_VertexIndex].tex_coord;
}
