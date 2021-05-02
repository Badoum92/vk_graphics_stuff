#include "global.glsl"

layout(location = 0) in vec2 in_position;
layout(location = 1) in vec3 in_color;

layout(location = 0) out vec3 out_color;

void main()
{
    gl_Position = vec4(in_position, 0.0, 1.0);
    // out_color = in_color;
    if (gl_VertexIndex == 0)
        out_color = global.color_0.xyz;
    else if (gl_VertexIndex == 1)
        out_color = global.color_1.xyz;
    else if (gl_VertexIndex == 2)
        out_color = global.color_2.xyz;
    else if (gl_VertexIndex == 3)
        out_color = global.color_3.xyz;
}
