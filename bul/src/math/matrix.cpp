#include "bul/math/matrix.h"

#include <cstring>
#include <cmath>

namespace bul
{
mat4f::mat4f()
{
    memset(data, 0, 16 * sizeof(float));
}

mat4f::mat4f(float val)
    : mat4f()
{
    data[0] = val;
    data[5] = val;
    data[10] = val;
    data[15] = val;
}

mat4f::mat4f(const float (&values)[16])
{
    memcpy(data, values, 16 * sizeof(float));
}

mat4f::mat4f(const mat4f& other)
{
    memcpy(data, other.data, 16 * sizeof(float));
}

mat4f& mat4f::operator=(const mat4f& other)
{
    memcpy(data, other.data, 16 * sizeof(float));
    return *this;
}

mat4f mat4f::identity()
{
    return mat4f{1};
}

void mat4f::operator+=(const mat4f& other)
{
    for (size_t i = 0; i < 16; ++i)
    {
        data[i] += other.data[i];
    }
}
void mat4f::operator-=(const mat4f& other)
{
    for (size_t i = 0; i < 16; ++i)
    {
        data[i] -= other.data[i];
    }
}
void mat4f::operator*=(float val)
{
    for (size_t i = 0; i < 16; ++i)
    {
        data[i] *= val;
    }
}

bool mat4f::operator==(const mat4f& other) const
{
    return memcmp(data, other.data, 16 * sizeof(float)) == 0;
}

vec4f& mat4f::operator[](size_t i)
{
    ASSERT(i < 4);
    return cols[i];
    // return *reinterpret_cast<vec4f*>(&data[i * 4]);
}
vec4f mat4f::operator[](size_t i) const
{
    ASSERT(i < 4);
    return cols[i];
    // return *reinterpret_cast<const vec4f*>(&data[i * 4]);
}

mat4f operator+(const mat4f& a, const mat4f& b)
{
    mat4f ret = a;
    ret += b;
    return ret;
}
mat4f operator-(const mat4f& a, const mat4f& b)
{
    mat4f ret = a;
    ret -= b;
    return ret;
}
mat4f operator*(const mat4f& a, const mat4f& b)
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
mat4f operator*(const mat4f& a, float f)
{
    mat4f ret = a;
    ret *= f;
    return ret;
}
vec4f operator*(const mat4f& a, vec4f v)
{
    vec4f ret;
    ret.x = a[0][0] * v[0] + a[1][0] * v[1] + a[2][0] * v[2] + a[3][0] * v[3];
    ret.y = a[0][1] * v[0] + a[1][1] * v[1] + a[2][1] * v[2] + a[3][1] * v[3];
    ret.z = a[0][2] * v[0] + a[1][2] * v[1] + a[2][2] * v[2] + a[3][2] * v[3];
    ret.w = a[0][3] * v[0] + a[1][3] * v[1] + a[2][3] * v[2] + a[3][3] * v[3];
    return ret;
}

mat4f inverse(const mat4f& m)
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

mat4f lookat(bul::vec3f pos, bul::vec3f target, bul::vec3f up, mat4f* inv)
{
    vec3f z = normalize(target - pos);
    vec3f x = normalize(cross(z, up));
    vec3f y = cross(x, z);
    mat4f ret{{x.x, x.y, x.z, -dot(pos, x), y.x, y.y, y.z, -dot(pos, y), -z.x, -z.y, -z.z, dot(pos, z), 0.0f,
                       0.0f, 0.0f, 1.0f}};
    if (inv)
    {
        *inv = mat4f{{x.x, y.x, -z.x, pos.x, x.y, y.y, -z.y, pos.y, x.z, y.z, -z.z, pos.z, 0.0f, 0.0f, 0.0f, 1.0f}};
    }
    return ret;
}

/*T const tanHalfFovy = tan(fovy / static_cast<T>(2));

		mat<4, 4, T, defaultp> Result(static_cast<T>(0));
		Result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
		Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
		Result[2][2] = zFar / (zNear - zFar);
		Result[2][3] = - static_cast<T>(1);
		Result[3][2] = -(zFar * zNear) / (zFar - zNear);
		return Result;*/

mat4f perspective(float fov, float aspect_ratio, float near, float far, mat4f* inv)
{
    float tan_half_fov = std::tan(fov / 2.0f);
    mat4f proj{{1.0f / (aspect_ratio * tan_half_fov), 0.0f, 0.0f, 0.0f, 0.0f, -1.0f / tan_half_fov, 0.0f, 0.0f, 0.0f, 0.0f, near / (far - near), -1.0f}};
}
} // namespace bul
