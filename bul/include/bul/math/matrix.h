#pragma once

#include "bul/bul.h"
#include "bul/math/vector.h"

#include <cstring>
#include <cmath>

namespace bul
{
struct mat4f
{
    union
    {
        float data[16];
        vec4f cols[4];
    };

    mat4f()
    {
        memset(data, 0, 16 * sizeof(float));
    }

    explicit mat4f(const float (&values)[16])
    {
        memcpy(data, values, 16 * sizeof(float));
    }

    mat4f(const mat4f& other)
    {
        memcpy(data, other.data, 16 * sizeof(float));
    }

    mat4f& operator=(const mat4f& other)
    {
        memcpy(data, other.data, 16 * sizeof(float));
        return *this;
    }

    static mat4f identity()
    {
        // clang-format off
        return mat4f({
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        });
        // clang-format on
    }

    void operator+=(const mat4f& other)
    {
        for (size_t i = 0; i < 16; ++i)
        {
            data[i] += other.data[i];
        }
    }

    void operator-=(const mat4f& other)
    {
        for (size_t i = 0; i < 16; ++i)
        {
            data[i] -= other.data[i];
        }
    }

    void operator*=(float val)
    {
        for (size_t i = 0; i < 16; ++i)
        {
            data[i] *= val;
        }
    }

    bool operator==(const mat4f& other) const
    {
        return memcmp(data, other.data, 16 * sizeof(float)) == 0;
    }

    vec4f& operator[](size_t i)
    {
        ASSERT(i < 4);
        return cols[i];
    }

    const vec4f& operator[](size_t i) const
    {
        ASSERT(i < 4);
        return cols[i];
    }
};

inline mat4f operator+(const mat4f& a, const mat4f& b)
{
    mat4f ret = a;
    ret += b;
    return ret;
}

inline mat4f operator-(const mat4f& a, const mat4f& b)
{
    mat4f ret = a;
    ret -= b;
    return ret;
}

inline mat4f operator*(const mat4f& a, const mat4f& b)
{
    mat4f res;
    res[0][0] = a[0][0] * b[0][0] + a[1][0] * b[0][1] + a[2][0] * b[0][2] + a[3][0] * b[0][3];
    res[0][1] = a[0][1] * b[0][0] + a[1][1] * b[0][1] + a[2][1] * b[0][2] + a[3][1] * b[0][3];
    res[0][2] = a[0][2] * b[0][0] + a[1][2] * b[0][1] + a[2][2] * b[0][2] + a[3][2] * b[0][3];
    res[0][3] = a[0][3] * b[0][0] + a[1][3] * b[0][1] + a[2][3] * b[0][2] + a[3][3] * b[0][3];

    res[1][0] = a[0][0] * b[1][0] + a[1][0] * b[1][1] + a[2][0] * b[1][2] + a[3][0] * b[1][3];
    res[1][1] = a[0][1] * b[1][0] + a[1][1] * b[1][1] + a[2][1] * b[1][2] + a[3][1] * b[1][3];
    res[1][2] = a[0][2] * b[1][0] + a[1][2] * b[1][1] + a[2][2] * b[1][2] + a[3][2] * b[1][3];
    res[1][3] = a[0][3] * b[1][0] + a[1][3] * b[1][1] + a[2][3] * b[1][2] + a[3][3] * b[1][3];

    res[2][0] = a[0][0] * b[2][0] + a[1][0] * b[2][1] + a[2][0] * b[2][2] + a[3][0] * b[2][3];
    res[2][1] = a[0][1] * b[2][0] + a[1][1] * b[2][1] + a[2][1] * b[2][2] + a[3][1] * b[2][3];
    res[2][2] = a[0][2] * b[2][0] + a[1][2] * b[2][1] + a[2][2] * b[2][2] + a[3][2] * b[2][3];
    res[2][3] = a[0][3] * b[2][0] + a[1][3] * b[2][1] + a[2][3] * b[2][2] + a[3][3] * b[2][3];

    res[3][0] = a[0][0] * b[3][0] + a[1][0] * b[3][1] + a[2][0] * b[3][2] + a[3][0] * b[3][3];
    res[3][1] = a[0][1] * b[3][0] + a[1][1] * b[3][1] + a[2][1] * b[3][2] + a[3][1] * b[3][3];
    res[3][2] = a[0][2] * b[3][0] + a[1][2] * b[3][1] + a[2][2] * b[3][2] + a[3][2] * b[3][3];
    res[3][3] = a[0][3] * b[3][0] + a[1][3] * b[3][1] + a[2][3] * b[3][2] + a[3][3] * b[3][3];
    return res;
}

inline mat4f operator*(const mat4f& a, float f)
{
    mat4f ret = a;
    ret *= f;
    return ret;
}

inline vec4f operator*(const mat4f& a, const vec4f& v)
{
    vec4f ret;
    ret.x = a[0][0] * v[0] + a[1][0] * v[1] + a[2][0] * v[2] + a[3][0] * v[3];
    ret.y = a[0][1] * v[0] + a[1][1] * v[1] + a[2][1] * v[2] + a[3][1] * v[3];
    ret.z = a[0][2] * v[0] + a[1][2] * v[1] + a[2][2] * v[2] + a[3][2] * v[3];
    ret.w = a[0][3] * v[0] + a[1][3] * v[1] + a[2][3] * v[2] + a[3][3] * v[3];
    return ret;
}

inline mat4f translation(const vec3f& v)
{
    // clang-format off
    return mat4f{{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        v.x , v.y , v.y , 1.0f,
    }};
    // clang-format on
}

inline mat4f scale(const vec3f& s)
{
    // clang-format off
    return mat4f{{
        s.x , 0.0f, 0.0f, 0.0f,
        0.0f, s.y , 0.0f, 0.0f,
        0.0f, 0.0f, s.z , 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    }};
    // clang-format on
}

inline mat4f rotation_x(float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    // clang-format off
    return mat4f{{
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, c,    s,    0.0f,
        0.0f, -s,   c,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    }};
    // clang-format on
}

inline mat4f rotation_y(float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    // clang-format off
    return mat4f({
        c,     s,    0.0f, 0.0f,
        -s,    c,    0.0f, 0.0f,
        0.0f,  0.0f, 1.0f, 0.0f,
        0.0f,  0.0f, 0.0f, 1.0f,
    });
    // clang-format on
}

inline mat4f rotation_z(float angle)
{
    float s = sin(angle);
    float c = cos(angle);
    // clang-format off
    return mat4f({
        c,    0.0f, -s,   0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        s,    0.0f, c,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    });
    // clang-format on
}

inline mat4f rotation(bul::vec4f quaternion)
{
    float qxx = quaternion.x * quaternion.x;
    float qyy = quaternion.y * quaternion.y;
    float qzz = quaternion.z * quaternion.z;
    float qxz = quaternion.x * quaternion.z;
    float qxy = quaternion.x * quaternion.y;
    float qyz = quaternion.y * quaternion.z;
    float qwx = quaternion.w * quaternion.x;
    float qwy = quaternion.w * quaternion.y;
    float qwz = quaternion.w * quaternion.z;

    // clang-format off
    return mat4f({
        1.0f - 2.0f * (qyy + qzz), 2.0f * (qxy + qwz),        2.0f * (qxz - qwy),        0.0f,
        2.0f * (qxy - qwz),        1.0f - 2.0f * (qxx + qzz), 2.0f * (qyz + qwx),        0.0f,
        2.0f * (qxz + qwy),        2.0f * (qyz - qwx),        1.0f - 2.0f * (qxx + qyy), 0.0f,
        0.0f,                      0.0f,                      0.0f,                      1.0f,
    });
    // clang-format on
}

inline mat4f inverse(const mat4f& m)
{
    mat4f inv;

    inv[0][0] = m[1][1] * m[2][2] * m[3][3] - m[1][1] * m[2][3] * m[3][2] - m[2][1] * m[1][2] * m[3][3]
        + m[2][1] * m[1][3] * m[3][2] + m[3][1] * m[1][2] * m[2][3] - m[3][1] * m[1][3] * m[2][2];

    inv[1][0] = -m[1][0] * m[2][2] * m[3][3] + m[1][0] * m[2][3] * m[3][2] + m[2][0] * m[1][2] * m[3][3]
        - m[2][0] * m[1][3] * m[3][2] - m[3][0] * m[1][2] * m[2][3] + m[3][0] * m[1][3] * m[2][2];

    inv[2][0] = m[1][0] * m[2][1] * m[3][3] - m[1][0] * m[2][3] * m[3][1] - m[2][0] * m[1][1] * m[3][3]
        + m[2][0] * m[1][3] * m[3][1] + m[3][0] * m[1][1] * m[2][3] - m[3][0] * m[1][3] * m[2][1];

    inv[3][0] = -m[1][0] * m[2][1] * m[3][2] + m[1][0] * m[2][2] * m[3][1] + m[2][0] * m[1][1] * m[3][2]
        - m[2][0] * m[1][2] * m[3][1] - m[3][0] * m[1][1] * m[2][2] + m[3][0] * m[1][2] * m[2][1];

    inv[0][1] = -m[0][1] * m[2][2] * m[3][3] + m[0][1] * m[2][3] * m[3][2] + m[2][1] * m[0][2] * m[3][3]
        - m[2][1] * m[0][3] * m[3][2] - m[3][1] * m[0][2] * m[2][3] + m[3][1] * m[0][3] * m[2][2];

    inv[1][1] = m[0][0] * m[2][2] * m[3][3] - m[0][0] * m[2][3] * m[3][2] - m[2][0] * m[0][2] * m[3][3]
        + m[2][0] * m[0][3] * m[3][2] + m[3][0] * m[0][2] * m[2][3] - m[3][0] * m[0][3] * m[2][2];

    inv[2][1] = -m[0][0] * m[2][1] * m[3][3] + m[0][0] * m[2][3] * m[3][1] + m[2][0] * m[0][1] * m[3][3]
        - m[2][0] * m[0][3] * m[3][1] - m[3][0] * m[0][1] * m[2][3] + m[3][0] * m[0][3] * m[2][1];

    inv[3][1] = m[0][0] * m[2][1] * m[3][2] - m[0][0] * m[2][2] * m[3][1] - m[2][0] * m[0][1] * m[3][2]
        + m[2][0] * m[0][2] * m[3][1] + m[3][0] * m[0][1] * m[2][2] - m[3][0] * m[0][2] * m[2][1];

    inv[0][2] = m[0][1] * m[1][2] * m[3][3] - m[0][1] * m[1][3] * m[3][2] - m[1][1] * m[0][2] * m[3][3]
        + m[1][1] * m[0][3] * m[3][2] + m[3][1] * m[0][2] * m[1][3] - m[3][1] * m[0][3] * m[1][2];

    inv[1][2] = -m[0][0] * m[1][2] * m[3][3] + m[0][0] * m[1][3] * m[3][2] + m[1][0] * m[0][2] * m[3][3]
        - m[1][0] * m[0][3] * m[3][2] - m[3][0] * m[0][2] * m[1][3] + m[3][0] * m[0][3] * m[1][2];

    inv[2][2] = m[0][0] * m[1][1] * m[3][3] - m[0][0] * m[1][3] * m[3][1] - m[1][0] * m[0][1] * m[3][3]
        + m[1][0] * m[0][3] * m[3][1] + m[3][0] * m[0][1] * m[1][3] - m[3][0] * m[0][3] * m[1][1];

    inv[3][2] = -m[0][0] * m[1][1] * m[3][2] + m[0][0] * m[1][2] * m[3][1] + m[1][0] * m[0][1] * m[3][2]
        - m[1][0] * m[0][2] * m[3][1] - m[3][0] * m[0][1] * m[1][2] + m[3][0] * m[0][2] * m[1][1];

    inv[0][3] = -m[0][1] * m[1][2] * m[2][3] + m[0][1] * m[1][3] * m[2][2] + m[1][1] * m[0][2] * m[2][3]
        - m[1][1] * m[0][3] * m[2][2] - m[2][1] * m[0][2] * m[1][3] + m[2][1] * m[0][3] * m[1][2];

    inv[1][3] = m[0][0] * m[1][2] * m[2][3] - m[0][0] * m[1][3] * m[2][2] - m[1][0] * m[0][2] * m[2][3]
        + m[1][0] * m[0][3] * m[2][2] + m[2][0] * m[0][2] * m[1][3] - m[2][0] * m[0][3] * m[1][2];

    inv[2][3] = -m[0][0] * m[1][1] * m[2][3] + m[0][0] * m[1][3] * m[2][1] + m[1][0] * m[0][1] * m[2][3]
        - m[1][0] * m[0][3] * m[2][1] - m[2][0] * m[0][1] * m[1][3] + m[2][0] * m[0][3] * m[1][1];

    inv[3][3] = m[0][0] * m[1][1] * m[2][2] - m[0][0] * m[1][2] * m[2][1] - m[1][0] * m[0][1] * m[2][2]
        + m[1][0] * m[0][2] * m[2][1] + m[2][0] * m[0][1] * m[1][2] - m[2][0] * m[0][2] * m[1][1];

    float det = m[0][0] * inv[0][0] + m[0][1] * inv[1][0] + m[0][2] * inv[2][0] + m[0][3] * inv[3][0];

    ASSERT(det != 0);

    det = 1.0f / det;

    for (size_t i = 0; i < 16; i++)
        inv.data[i] *= det;

    return inv;
}

inline mat4f lookat(bul::vec3f pos, bul::vec3f target, bul::vec3f up, mat4f* inv = nullptr)
{
    vec3f z = normalize(target - pos);
    vec3f x = normalize(cross(z, up));
    vec3f y = cross(x, z);

    // clang-format off
    mat4f ret{{
        x.x,  x.y,  x.z,  -dot(pos, x),
        y.x,  y.y,  y.z,  -dot(pos, y),
        -z.x, -z.y, -z.z, dot(pos, z),
        0.0f, 0.0f, 0.0f, 1.0f,
    }};

    if (inv)
    {
        *inv = mat4f{{
            x.x,  y.x,  -z.x, pos.x,
            x.y,  y.y,  -z.y, pos.y,
            x.z,  y.z,  -z.z, pos.z,
            0.0f, 0.0f, 0.0f, 1.0f,
        }};
    }
    // clang-format on

    return ret;
}

inline mat4f perspective(float fov_y, float aspect_ratio, float _near, float _far, mat4f* inv = nullptr)
{
    float A = std::tan(fov_y / 2.0f);
    float B = A * aspect_ratio;

    // clang-format off
    mat4f proj{{
        1.0f / B, 0.0f,      0.0f,                0.0f,
        0.0f,     -1.0f / A, 0.0f,                0.0f,
        0.0f,     0.0f,      _near / (_far - _near), (_far * _near) / (_far - _near),
        0.0f,     0.0f,      -1.0f,               0.0f,
    }};

    if (inv)
    {
        *inv = mat4f{{
            B,    0.0f, 0.0f,                        0.0f,
            0.0f, -A,   0.0f,                        0.0f,
            0.0f, 0.0f, 0.0f,                        -1.0f,
            0.0f, 0.0f, (_far - _near) / (_far * _near), 1.0f / _far,
        }};
    }
    // clang-format on

    return proj;
}
} // namespace bul
