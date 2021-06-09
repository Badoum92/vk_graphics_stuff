#include "global.glsl"
#include "voxel.glsl"

layout(location = 0) out flat int voxel_index;

layout(set = 1, binding = 0) buffer VoxelBuffer
{
    Voxel voxels[];
};

struct Vertex
{
    vec3 pos;
    float _pad0;
    vec3 normal;
    float _pad1;
};

layout(set = 1, binding = 1) buffer VertexBuffer
{
    Vertex vertices[];
};

void main()
{
    vec3 world_pos = vertices[gl_VertexIndex].pos + voxels[gl_InstanceIndex].pos.xyz;
    gl_Position = global.proj * global.view * vec4(world_pos, 1.0);
    voxel_index = gl_InstanceIndex;
}
