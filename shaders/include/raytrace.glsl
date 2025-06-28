#ifndef RAYTRACE_H
#define RAYTRACE_H

#include "global.glsl"

struct ray
{
    vec3 pos;
    vec3 dir;
    vec3 inv_dir;
};

ray ray_gen(uvec2 pixel, uvec2 resolution, vec4 position, mat4 inv_view_proj, bool anti_aliasing, inout uint rng)
{
    vec2 jitter = vec2(0);
    if (anti_aliasing)
    {
        jitter = vec2(random_float_01(rng), random_float_01(rng)) - 0.5f;
    }
    vec2 uv = (vec2(pixel) + jitter) / vec2(resolution);

    vec4 target = vec4(uv * 2.0f - 1.0f, 0.1f, 1.0f);
    target = inv_view_proj * target;
    target /= target.w;

    ray ray;
    ray.pos = position.xyz;
    ray.dir = normalize(target.xyz - ray.pos);
    ray.inv_dir = 1.0f / ray.dir;

    return ray;
}

bool ray_box_intersection(ray ray, vec3 bmin, vec3 bmax, out float _tmin, out float _tmax)
{
    float t1 = (bmin[0] - ray.pos[0]) * ray.inv_dir[0];
    float t2 = (bmax[0] - ray.pos[0]) * ray.inv_dir[0];

    float tmin = min(t1, t2);
    float tmax = max(t1, t2);

    for (uint i = 1; i < 3; ++i)
    {
        t1 = (bmin[i] - ray.pos[i]) * ray.inv_dir[i];
        t2 = (bmax[i] - ray.pos[i]) * ray.inv_dir[i];

        tmin = max(tmin, min(t1, t2));
        tmax = min(tmax, max(t1, t2));
    }

    tmin = max(tmin, 0.0f);
    _tmin = tmin;
    _tmax = tmax;
    return tmax > tmin;
}

#endif
