#include "global.glsl"

struct vertex
{
    vec3 position;
    uint color;
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

INPUT(0, vec4 in_color);

OUTPUT(0, vec4 out_color);

void main()
{
    out_color = in_color;
}
