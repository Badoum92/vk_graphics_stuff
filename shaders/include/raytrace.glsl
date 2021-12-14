#ifndef RAYTRACE_H
#define RAYTRACE_H

#include "global.glsl"

struct Ray
{
    vec3 origin;
    vec3 dir;
    vec3 inv_dir;
};

Ray ray_gen(ivec2 pixel, bool anti_aliasing, uint rng_state)
{
    vec2 jitter = vec2(0);
    if (anti_aliasing)
    {
        jitter = vec2(random_float_01(rng_state), random_float_01(rng_state)) - 0.5f;
    }
    vec2 uv = (vec2(pixel) + jitter) / vec2(global.resolution);

    vec4 target = vec4(uv * 2.0f - 1.0f, 0.1f, 1.0f);
    target = global.inv_view * global.inv_proj * target;
    target /= target.w;

    Ray ray;
    ray.origin = global.camera_pos.xyz;
    ray.dir = normalize(target.xyz - ray.origin);
    ray.inv_dir = 1.0f / ray.dir;

    return ray;
}

bool ray_box_intersection(Ray ray, vec3 bmin, vec3 bmax, inout float tmin, inout float tmax)
{
    float t1 = (bmin[0] - ray.origin[0]) * ray.inv_dir[0];
    float t2 = (bmax[0] - ray.origin[0]) * ray.inv_dir[0];

    tmin = min(t1, t2);
    tmax = max(t1, t2);

    for (uint i = 1; i < 3; ++i)
    {
        t1 = (bmin[i] - ray.origin[i]) * ray.inv_dir[i];
        t2 = (bmax[i] - ray.origin[i]) * ray.inv_dir[i];

        tmin = max(tmin, min(t1, t2));
        tmax = min(tmax, max(t1, t2));
    }

    tmin = max(tmin, 0.0f);
    return tmax > tmin;
}

#endif
