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

BUFFER(uniform_buffer)
{
    mat4 mvp;
};

PUSH_CONSTANT(push_constant)
{
    vertex_buffer vb;
    uniform_buffer ubo;
};

INPUT(0, vec4 in_normal);
INPUT(1, vec2 in_uv);

OUTPUT(0, vec4 out_color);

void main()
{
    out_color = (in_normal + vec4(1.0f)) * 0.5f;
}
