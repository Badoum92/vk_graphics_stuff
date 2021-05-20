#include "global.glsl"
#include "voxel.glsl"

layout(location = 0) out int voxel_index;

layout(set = 1, binding = 0) buffer VoxelBuffer
{
    Voxel voxels[];
};

void quadricProj(in vec3 osPosition, in float voxelSize, in mat4 objectToScreenMatrix, in vec2 halfScreenSize,
                 inout vec4 position, inout float pointSize)
{
    const vec4 quadricMat = vec4(1.0, 1.0, 1.0, -1.0);
    float sphereRadius = voxelSize * 1.732051;
    vec4 sphereCenter = vec4(osPosition.xyz, 1.0);
    mat4 modelViewProj = transpose(objectToScreenMatrix);
    mat3x4 matT = mat3x4(mat3(modelViewProj[0].xyz, modelViewProj[1].xyz, modelViewProj[3].xyz) * sphereRadius);
    matT[0].w = dot(sphereCenter, modelViewProj[0]);
    matT[1].w = dot(sphereCenter, modelViewProj[1]);
    matT[2].w = dot(sphereCenter, modelViewProj[3]);
    mat3x4 matD = mat3x4(matT[0] * quadricMat, matT[1] * quadricMat, matT[2] * quadricMat);
    vec4 eqCoefs = vec4(dot(matD[0], matT[2]), dot(matD[1], matT[2]), dot(matD[0], matT[0]), dot(matD[1], matT[1]))
        / dot(matD[2], matT[2]);
    vec4 outPosition = vec4(eqCoefs.x, eqCoefs.y, 0.0, 1.0);
    vec2 AABB = sqrt(eqCoefs.xy * eqCoefs.xy - eqCoefs.zw);
    AABB *= halfScreenSize * 2.0f;
    position.xy = outPosition.xy * position.w;
    pointSize = max(AABB.x, AABB.y);
}

void main()
{
    voxel_index = gl_VertexIndex;
    mat4 view_proj = global.proj * global.view;
    vec2 half_res = global.resolution / 2.0f;
    vec4 position = view_proj * voxels[voxel_index].pos;
    float point_size = 1.0f;

    quadricProj(voxels[voxel_index].pos.xyz, voxel_size, view_proj, half_res, position, point_size);

    gl_Position = position / position.w;
    gl_PointSize = point_size;

    if (gl_Position.z >= 1.0)
    {
        gl_Position = vec4(-1);
        return;
    }

    float stochasticCoverage = gl_PointSize * gl_PointSize;
    if ((stochasticCoverage < 0.8) && ((voxel_index & 0xffff) > stochasticCoverage * (0xffff / 0.8)))
    {
        gl_Position = vec4(-1);
        return;
    }
}
