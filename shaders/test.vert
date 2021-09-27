#include "test.glsl"

/* layout(set = 1, binding = 0) buffer VertexBuffer
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
} */

layout(set = 1, binding = 0) buffer VertexBuffer
{
    Vertex vertices[];
};

layout (set = 1, binding = 1) uniform MeshUniform
{
    mat4 transform;
} mesh_uniform;

layout(location = 0) out vec4 world_pos;
layout(location = 1) out vec4 normal;
layout(location = 2) out vec2 uv_0;

void main()
{
    mat4 mvp = global.proj * global.view * mesh_uniform.transform;
    gl_Position = mvp * vertices[gl_VertexIndex].position;
    world_pos = mesh_uniform.transform * vertices[gl_VertexIndex].position;
    normal = vertices[gl_VertexIndex].normal;
    uv_0 = vertices[gl_VertexIndex].uv_0;
}
