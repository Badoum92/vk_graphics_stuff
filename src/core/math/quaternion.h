#pragma once

#include "core/math/math.h"
#include "core/math/vector.h"

struct quaternion
{
    float operator[](uint32_t i) const
    {
        return (&x)[i];
    }

    float& operator[](uint32_t i)
    {
        return (&x)[i];
    }

    float x;
    float y;
    float z;
    float w;
};

bool operator==(const quaternion& a, const quaternion& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

inline quaternion quat_identity()
{
    return {0.0f, 0.0f, 0.0f, 1.0f};
}

inline float quat_dot(quaternion a, quaternion b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline float quat_norm(quaternion q)
{
    return sqrtf(quat_dot(q, q));
}

inline quaternion quat_normalize(quaternion q)
{
    float n = quat_norm(q);
    return {q.x / n, q.y / n, q.z / n, q.w / n};
}

inline quaternion quat_slerp(quaternion a, quaternion b, float t)
{
    float dot = quat_dot(a, b);
    if (dot < 0.0f)
    {
        dot = -dot;
        b.x = -b.x;
        b.y = -b.y;
        b.z = -b.z;
        b.w = -b.w;
    }

    float omega = acosf(MIN(MAX(dot, 0.0f), 1.0f));
    if (omega <= 1.175494351e-38f)
        return a;

    float rcp_so = 1.0f / sinf(omega);
    float af = sinf((1.0f - t) * omega) * rcp_so;
    float bf = sinf(t * omega) * rcp_so;

    float x = af * a.x + bf * b.x;
    float y = af * a.y + bf * b.y;
    float z = af * a.z + bf * b.z;
    float w = af * a.w + bf * b.w;
    float rcp_len = 1.0f / sqrtf(x * x + y * y + z * z + w * w);

    return {x * rcp_len, y * rcp_len, z * rcp_len, w * rcp_len};
}

inline vec3f quat_rotate_vec(quaternion q, vec3f v)
{
    float xy = q.x * v.y - q.y * v.x;
    float xz = q.x * v.z - q.z * v.x;
    float yz = q.y * v.z - q.z * v.y;
    vec3f ret;
    ret.x = 2.0f * (+q.w * yz + q.y * xy + q.z * xz) + v.x;
    ret.y = 2.0f * (-q.x * xy - q.w * xz + q.z * yz) + v.y;
    ret.z = 2.0f * (-q.x * xz - q.y * yz + q.w * xy) + v.z;
    return ret;
}

inline quaternion quat_from_euler(vec3f v)
{
    float vx = v.x * RADIANS(0.5f);
    float vy = v.y * RADIANS(0.5f);
    float vz = v.z * RADIANS(0.5f);
    float cx = cosf(vx);
    float sx = sinf(vx);
    float cy = cosf(vy);
    float sy = sinf(vy);
    float cz = cosf(vz);
    float sz = sinf(vz);
    quaternion q;
    q.x = -cx * sy * sz + cy * cz * sx;
    q.y = cx * cz * sy + cy * sx * sz;
    q.z = cx * cy * sz - cz * sx * sy;
    q.w = cx * cy * cz + sx * sy * sz;
    return q;
}

inline vec3f quat_to_euler(quaternion q)
{
    const float epsilon = 0.9999999f;
    vec3f v;
    float t;
    t = 2.0f * (q.w * q.y - q.x * q.z);
    if (fabs(t) < epsilon)
    {
        v.y = sinf(t);
        v.z = atan2f(2.0f * (q.w * q.z + q.x * q.y), 2.0f * (q.w * q.w + q.x * q.x) - 1.0f);
        v.x = -atan2f(-2.0f * (q.w * q.x + q.y * q.z), 2.0f * (q.w * q.w + q.z * q.z) - 1.0f);
    }
    else
    {
        v.y = copysignf(math_half_pi, t);
        v.z = atan2f(-2.0f * t * (q.w * q.x - q.y * q.z), t * (2.0f * q.w * q.y + 2.0f * q.x * q.z));
        v.x = 0.0f;
    }
    v.x = DEGREES(v.x);
    v.y = DEGREES(v.y);
    v.z = DEGREES(v.z);
    return v;
}