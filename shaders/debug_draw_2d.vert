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
    uvec2 resolution;
    vertex_buffer vb;
};

OUTPUT(0, vec4 out_color);

void main()
{
    out_color = rgba_from_uint(vb.vertices[VERTEX_INDEX].color);
    gl_Position = vec4((vb.vertices[VERTEX_INDEX].position.xy / resolution) * 2.0f - 1.0f, 0.0f, 1.0f);
}
