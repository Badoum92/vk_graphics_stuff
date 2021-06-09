#include "global.glsl"

layout(location = 0) out vec3 out_normal;
layout(location = 1) out vec3 out_position;

struct Vertex
{
    vec3 pos;
    float _pad0;
    vec3 normal;
    float _pad1;
};

layout(set = 1, binding = 0) buffer VertexBuffer
{
    Vertex vertices[];
};

void main()
{
    gl_Position = global.proj * global.view * vec4(vertices[gl_VertexIndex].pos, 1.0);
    out_normal = vertices[gl_VertexIndex].normal;
    out_position = vertices[gl_VertexIndex].pos;
}
