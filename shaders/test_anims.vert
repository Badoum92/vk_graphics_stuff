#include "global.glsl"

struct vertex
{
    vec4 position;
    vec4 normal;
    vec2 uv;
    vec2 _padding;
};

BUFFER(vertex_buffer)
{
    vertex vertices[];
};

PUSH_CONSTANT(push_constant)
{
    mat4 mvp;
    vertex_buffer vb;
};

OUTPUT(0, vec4 out_normal);
OUTPUT(1, vec2 out_uv);

void main()
{
    out_normal = vb.vertices[VERTEX_INDEX].normal;
    out_uv = vb.vertices[VERTEX_INDEX].uv;
    gl_Position = mvp * vb.vertices[VERTEX_INDEX].position;
}
