#include "global.glsl"
#include "voxel.glsl"
#include "raytrace.glsl"

layout(location = 0) in flat int voxel_index;

layout(location = 0) out vec4 frag_color;

layout(set = 1, binding = 0) buffer VoxelBuffer
{
    Voxel voxels[];
};

vec3 get_color_for_ray(in Ray ray)
{
    vec3 ret = vec3(0.0f, 0.0f, 0.0f);

    Voxel vox = voxels[voxel_index];

    Box box;
    box.center = vox.pos.xyz + vec3(-voxel_size, voxel_size, voxel_size);
    box.radius = vec3(voxel_size);
    box.inv_radius = 1.0f / box.radius;

    float dist = 0.0f;
    // if (!test_aabb(vox.pos.xyz + vec3(0.0f, 0.0f, -voxel_size) * 2.0f, vox.pos.xyz + vec3(voxel_size, voxel_size, 0.0f) * 2.0f, ray, dist))
    // if (!test_aabb(vox.pos.xyz + vec3(0.0f, 0.0f, 0.0f) * 2.0f, vox.pos.xyz + vec3(voxel_size, voxel_size, voxel_size) * 2.0f, ray, dist))
    /* if (!test_aabb(vox.pos.xyz, vox.pos.xyz + vec3(voxel_size) * 2.0f, ray, dist))
    {
        discard;
    }
    ret = vox.color.rgb; */
    if (test_aabb(vox.pos.xyz, vox.pos.xyz + vec3(voxel_size) * 2.0f, ray, dist))
    {
        return vox.color.rgb;
    }
    else
    {
        discard;
        // return vec3(1);
    }

    /* float dist = 0;
    vec3 normal = vec3(0);
    if (!test_box(box, ray, dist, normal, true, false))
    {
        discard;
    } */

    /* vec4 hit_pos = vec4(ray.pos + ray.dir * dist, 1.0f);
    vec4 screen_space_hit = global.proj * global.view * hit_pos;
    gl_FragDepth = screen_space_hit.z / screen_space_hit.w; */

    /* ret = vox.color.rgb;
    if (normal.x < 0 || normal.y < 0 || normal.z < 0)
    {
        ret *= 0.8f;
    } */

    return ret;
}

void main()
{
    /* frag_color = vec4(voxels[voxel_index].color);
    return; */

    vec2 uv = vec2(gl_FragCoord.xy) / global.resolution;

    // frag_color = vec4(uv, 0.0f, 1.0f);
    // return;

    vec4 target = vec4(uv * 2.0f - 1.0f, 0.0f, 1.0f);
    target = global.inv_view * global.inv_proj * target;
    target /= target.w;

    Ray ray;
    ray.pos = global.camera_pos;
    ray.dir = normalize(target.xyz - ray.pos);
    ray.inv_dir = 1.0f / ray.dir;

    frag_color = vec4(get_color_for_ray(ray), 1.0f);
}
