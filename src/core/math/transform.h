#pragma once

#include "core/math/vector.h"
#include "core/math/quaternion.h"
#include "core/math/matrix.h"

struct transform
{
    vec3f translation;
    quaternion rotation;
    vec3f scale;
};

inline transform transform_identity()
{
    transform t;
    t.translation = {0.0f, 0.0f, 0.0f};
    t.rotation = quat_identity();
    t.scale = {1.0f, 1.0f, 1.0f};
    return t;
}

inline mat4f transform_to_matrix(const transform& t)
{
    float xx = t.rotation.x * t.rotation.x;
    float yy = t.rotation.y * t.rotation.y;
    float zz = t.rotation.z * t.rotation.z;
    float xy = t.rotation.x * t.rotation.y;
    float xz = t.rotation.x * t.rotation.z;
    float yz = t.rotation.y * t.rotation.z;
    float xw = t.rotation.x * t.rotation.w;
    float yw = t.rotation.y * t.rotation.w;
    float zw = t.rotation.z * t.rotation.w;

    float sx = t.scale.x * 2.0f;
    float sy = t.scale.y * 2.0f;
    float sz = t.scale.z * 2.0f;

    mat4f m;
    m[0][0] = t.scale.x - sx * (yy + zz);
    m[0][1] = sx * (xy + zw);
    m[0][2] = sx * (xz - yw);
    m[0][3] = 0.0f;
    m[1][0] = sy * (xy - zw);
    m[1][1] = t.scale.y - sy * (xx + zz);
    m[1][2] = sy * (yz + xw);
    m[1][3] = 0.0f;
    m[2][0] = sz * (xz + yw);
    m[2][1] = sz * (yz - xw);
    m[2][2] = t.scale.z - sz * (xx + yy);
    m[2][3] = 0.0f;
    m[3][0] = t.translation.x;
    m[3][1] = t.translation.y;
    m[3][2] = t.translation.z;
    m[3][3] = 1.0f;
    return m;
}

inline transform transform_from_matrix(const mat4f& m)
{
    float det = mat4_determinant(m);

    transform t;
    t.translation = m[3];
    t.scale.x = vec_length(m[0]);
    t.scale.y = vec_length(m[1]);
    t.scale.z = vec_length(m[2]);

    float sign_x = 1.0f;
    float sign_y = 1.0f;
    float sign_z = 1.0f;
    if (det < 0.0f)
    {
        if (t.scale.x > 0.0f)
            sign_x = -1.0f;
        else if (t.scale.y > 0.0f)
            sign_y = -1.0f;
        else if (t.scale.z > 0.0f)
            sign_z = -1.0f;
    }

    vec3f x = m[0] * (t.scale.x > 0.0f ? sign_x / t.scale.x : 0.0f);
    vec3f y = m[1] * (t.scale.y > 0.0f ? sign_y / t.scale.y : 0.0f);
    vec3f z = m[2] * (t.scale.z > 0.0f ? sign_z / t.scale.z : 0.0f);
    float trace = x.x + y.y + z.z;
    if (trace > 0.0f)
    {
        float a = (float)sqrtf(MAX(0.0f, trace + 1.0f)), b = (a != 0.0f) ? 0.5f / a : 0.0f;
        t.rotation.x = (y.z - z.y) * b;
        t.rotation.y = (z.x - x.z) * b;
        t.rotation.z = (x.y - y.x) * b;
        t.rotation.w = 0.5f * a;
    }
    else if (x.x > y.y && x.x > z.z)
    {
        float a = (float)sqrtf(MAX(0.0f, 1.0f + x.x - y.y - z.z)), b = (a != 0.0f) ? 0.5f / a : 0.0f;
        t.rotation.x = 0.5f * a;
        t.rotation.y = (y.x + x.y) * b;
        t.rotation.z = (z.x + x.z) * b;
        t.rotation.w = (y.z - z.y) * b;
    }
    else if (y.y > z.z)
    {
        float a = (float)sqrtf(MAX(0.0f, 1.0f - x.x + y.y - z.z)), b = (a != 0.0f) ? 0.5f / a : 0.0f;
        t.rotation.x = (y.x + x.y) * b;
        t.rotation.y = 0.5f * a;
        t.rotation.z = (z.y + y.z) * b;
        t.rotation.w = (z.x - x.z) * b;
    }
    else
    {
        float a = (float)sqrtf(MAX(0.0f, 1.0f - x.x - y.y + z.z)), b = (a != 0.0f) ? 0.5f / a : 0.0f;
        t.rotation.x = (z.x + x.z) * b;
        t.rotation.y = (z.y + y.z) * b;
        t.rotation.z = 0.5f * a;
        t.rotation.w = (x.y - y.x) * b;
    }

    float len = t.rotation.x * t.rotation.x + t.rotation.y * t.rotation.y + t.rotation.z * t.rotation.z
        + t.rotation.w * t.rotation.w;
    if (fabs(len - 1.0f) > 1.0842021795674597e-19f)
    {
        if (fabs(len) <= 1.0842021795674597e-19f)
        {
            t.rotation = quat_identity();
        }
        else
        {
            t.rotation.x /= len;
            t.rotation.y /= len;
            t.rotation.z /= len;
            t.rotation.w /= len;
        }
    }

    t.scale.x *= sign_x;
    t.scale.y *= sign_y;
    t.scale.z *= sign_z;

    return t;
}