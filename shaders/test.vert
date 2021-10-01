#include "test.glsl"

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
