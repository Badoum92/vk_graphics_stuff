#ifndef RAYTRACE_H
#define RAYTRACE_H

struct Ray
{
    vec3 origin;
    vec3 dir;
    vec3 inv_dir;
};

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
