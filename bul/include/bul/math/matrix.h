#pragma once

#include "bul/bul.h"
#include "bul/math/vector.h"

#include <string.h>
#include <math.h>
#include <intrin.h>

namespace bul
{
struct mat4f
{
    float data[4 * 4];

    static mat4f create(const float (&values)[16])
    {
        mat4f m;
        for (uint32_t x = 0; x < 4; ++x)
        {
            for (uint32_t y = 0; y < 4; ++y)
            {
                m[x][y] = values[y * 4 + x];
            }
        }
        return m;
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
        for (uint32_t i = 0; i < 16; ++i)
        {
            data[i] += other.data[i];
        }
    }

    void operator-=(const mat4f& other)
    {
        for (uint32_t i = 0; i < 16; ++i)
        {
            data[i] -= other.data[i];
        }
    }

    void operator*=(float val)
    {
        for (uint32_t i = 0; i < 16; ++i)
        {
            data[i] *= val;
        }
    }

    bool operator==(const mat4f& other) const
    {
        return memcmp(data, other.data, 16 * sizeof(float)) == 0;
    }

    vec4f& operator[](uint32_t i)
    {
        ASSERT(i < 4);
        return ((vec4f*)data)[i];
    }

    const vec4f& operator[](uint32_t i) const
    {
        ASSERT(i < 4);
        return ((vec4f*)data)[i];
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
    mat4f c;
    __m128 col[4];
    __m128 sum[4];
    for (uint32_t i = 0; i < 4; i++)
    {
        col[i] = _mm_loadu_ps(&a.data[i * 4]);
    }
    for (uint32_t i = 0; i < 4; i++)
    {
        sum[i] = _mm_setzero_ps();
        for (uint32_t j = 0; j < 4; j++)
        {
            sum[i] = _mm_add_ps(_mm_mul_ps(_mm_set1_ps(b.data[i * 4 + j]), col[j]), sum[i]);
        }
    }
    for (uint32_t i = 0; i < 4; i++)
    {
        _mm_storeu_ps(&c.data[i * 4], sum[i]);
    }
    return c;
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
    __m128 _v = _mm_loadu_ps(&v.x);
    __m128 _a = _mm_mul_ps(_mm_loadu_ps(&a.data[0 * 4]), _v);
    __m128 _b = _mm_mul_ps(_mm_loadu_ps(&a.data[1 * 4]), _v);
    __m128 _c = _mm_mul_ps(_mm_loadu_ps(&a.data[2 * 4]), _v);
    __m128 _d = _mm_mul_ps(_mm_loadu_ps(&a.data[3 * 4]), _v);
    _mm_storeu_ps(&ret.x, _mm_hadd_ps(_mm_hadd_ps(_a, _b), _mm_hadd_ps(_c, _d)));
    return ret;
}

inline mat4f translation(const vec3f& v)
{
    // clang-format off
    return mat4f::create({
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        v.x , v.y , v.y , 1.0f,
    });
    // clang-format on
}

inline mat4f scale(const vec3f& s)
{
    // clang-format off
    return mat4f::create({
        s.x , 0.0f, 0.0f, 0.0f,
        0.0f, s.y , 0.0f, 0.0f,
        0.0f, 0.0f, s.z , 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    });
    // clang-format on
}

inline mat4f rotation_x(float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    // clang-format off
    return mat4f::create({
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, c,    -s,   0.0f,
        0.0f, s,    c,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    });
    // clang-format on
}

inline mat4f rotation_y(float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    // clang-format off
    return mat4f::create({
        c,    0.0f, s,    0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        -s,   0.0f, c,    0.0f,
        0.0f, 0.0f, 0.0f, 1.0f,
    });
    // clang-format on
}

inline mat4f rotation_z(float angle)
{
    float s = sinf(angle);
    float c = cosf(angle);
    // clang-format off
    return mat4f::create({
        c,     -s,   0.0f, 0.0f,
        s,     c,    0.0f, 0.0f,
        0.0f,  0.0f, 1.0f, 0.0f,
        0.0f,  0.0f, 0.0f, 1.0f,
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

    for (uint32_t i = 0; i < 16; i++)
    {
        inv.data[i] *= det;
    }

    return inv;
}

inline mat4f lookat(bul::vec3f pos, bul::vec3f target, bul::vec3f up, mat4f* inv = nullptr)
{
    vec3f z = normalize(target - pos);
    vec3f x = normalize(cross(z, up));
    vec3f y = cross(x, z);

    // clang-format off
    mat4f ret = mat4f::create({
        x.x,  x.y,  x.z,  -dot(pos, x),
        y.x,  y.y,  y.z,  -dot(pos, y),
        -z.x, -z.y, -z.z, dot(pos, z),
        0.0f, 0.0f, 0.0f, 1.0f,
    });

    if (inv)
    {
        *inv = mat4f::create({
            x.x,  y.x,  -z.x, pos.x,
            x.y,  y.y,  -z.y, pos.y,
            x.z,  y.z,  -z.z, pos.z,
            0.0f, 0.0f, 0.0f, 1.0f,
        });
    }
    // clang-format on

    return ret;
}

inline mat4f perspective(float fov_y, float aspect_ratio, float _near, float _far, mat4f* inv = nullptr)
{
    float focal_length = 1.0f / tanf(fov_y * 0.5f);
    float w = focal_length / aspect_ratio;
    float h = -focal_length;
    float a = _near / (_far - _near);
    float b = (_far * _near) / (_far - _near);

    // clang-format off
    mat4f proj = mat4f::create({
        w,    0.0f, 0.0f,  0.0f,
        0.0f, h,    0.0f,  0.0f,
        0.0f, 0.0f, a,     b,
        0.0f, 0.0f, -1.0f, 0.0f,
    });

    if (inv)
    {
        *inv = mat4f::create({
            1.0f / w, 0.0f,     0.0f,     0.0f,
            0.0f,     1.0f / h, 0.0f,     0.0f,
            0.0f,     0.0f,     0.0f,     -1.0f,
            0.0f,     0.0f,     1.0f / b, a / b,
        });
    }
    // clang-format on

    return proj;
}
} // namespace bul
