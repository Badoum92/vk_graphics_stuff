#include "test.glsl"

layout(set = 1, binding = 0) buffer VertexBuffer
{
    vec4 vertices[];
};

layout(set = 1, binding = 1) buffer ColorBuffer
{
    vec4 colors[];
};

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 position;

void main()
{
    gl_Position = global.proj * global.view * vec4(vertices[gl_VertexIndex].xyz, 1.0);
    position = vertices[gl_VertexIndex].xy + 0.5;
    fragColor = colors[gl_VertexIndex].rgb;
}
