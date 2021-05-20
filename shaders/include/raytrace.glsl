#ifndef RAYTRACE_H
#define RAYTRACE_H

struct Ray
{
    vec3 pos;
    vec3 dir;
    vec3 inv_dir;
};

struct Box
{
    vec3 center;
    vec3 radius;
    vec3 inv_radius;
    vec3 rot;
};

bool test_box(Box box, Ray ray, out float dist, out vec3 normal, const bool can_start_in_box,
                     const bool oriented)
{
    ray.pos = ray.pos - box.center;
    if (oriented)
    {
        ray.dir *= box.rot;
        ray.pos *= box.rot;
    }
    float winding = can_start_in_box && (max(abs(ray.pos) * box.inv_radius) < 1.0) ? -1 : 1;
    vec3 sgn = -sign(ray.dir);
    // Distance to plane
    vec3 d = box.radius * winding * sgn - ray.pos;
    if (oriented)
        d /= ray.dir;
    else
        d *= ray.inv_dir;
#define TEST(U, VW) (d.U >= 0.0) && all(lessThan(abs(ray.pos.VW + ray.dir.VW * d.U), box.radius.VW))
    bvec3 test = bvec3(TEST(x, yz), TEST(y, zx), TEST(z, xy));
    sgn = test.x ? vec3(sgn.x, 0, 0) : (test.y ? vec3(0, sgn.y, 0) : vec3(0, 0, test.z ? sgn.z : 0));
#undef TEST
    dist = (sgn.x != 0) ? d.x : ((sgn.y != 0) ? d.y : d.z);
    normal = oriented ? (box.rot * sgn) : sgn;
    return (sgn.x != 0) || (sgn.y != 0) || (sgn.z != 0);
}

#endif
