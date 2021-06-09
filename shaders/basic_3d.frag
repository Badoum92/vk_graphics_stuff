#include "global.glsl"

layout(location = 0) in vec3 in_normal;
layout(location = 1) in vec3 in_position;

layout(location = 0) out vec4 out_color;

void main()
{
    // out_color = vec4((in_normal + 1.0f) / 2.0f, 1.0f);
    out_color = vec4(in_position, 1.0f) * vec4(-1, 1, 1, 1);
}
