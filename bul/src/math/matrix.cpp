#include "bul/math/matrix.h"

#include <cstring>

namespace bul
{
mat4f::mat4f()
{
    memset(data, 0, 16 * sizeof(float));
}

mat4f::mat4f(float val)
{
    memset(data, 0, 16 * sizeof(float));
    data[0] = val;
    data[5] = val;
    data[10] = val;
    data[15] = val;
}

mat4f::mat4f(const float values[16])
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

float& mat4f::operator[](size_t i)
{
    return data[i];
}
float mat4f::operator[](size_t i) const
{
    return data[i];
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
    res[0] = a[0] * b[0] + a[4] * b[1] + a[8] * b[2] + a[12] * b[3];
    res[1] = a[1] * b[0] + a[5] * b[1] + a[9] * b[2] + a[13] * b[3];
    res[2] = a[2] * b[0] + a[6] * b[1] + a[10] * b[2] + a[14] * b[3];
    res[3] = a[3] * b[0] + a[7] * b[1] + a[11] * b[2] + a[15] * b[3];

    res[4] = a[0] * b[4] + a[4] * b[5] + a[8] * b[6] + a[12] * b[7];
    res[5] = a[1] * b[4] + a[5] * b[5] + a[9] * b[6] + a[13] * b[7];
    res[6] = a[2] * b[4] + a[6] * b[5] + a[10] * b[6] + a[14] * b[7];
    res[7] = a[3] * b[4] + a[7] * b[5] + a[11] * b[6] + a[15] * b[7];

    res[8] = a[0] * b[8] + a[4] * b[9] + a[8] * b[10] + a[12] * b[11];
    res[9] = a[1] * b[8] + a[5] * b[9] + a[9] * b[10] + a[13] * b[11];
    res[10] = a[2] * b[8] + a[6] * b[9] + a[10] * b[10] + a[14] * b[11];
    res[11] = a[3] * b[8] + a[7] * b[9] + a[11] * b[10] + a[15] * b[11];

    res[12] = a[0] * b[12] + a[4] * b[13] + a[8] * b[14] + a[12] * b[15];
    res[13] = a[1] * b[12] + a[5] * b[13] + a[9] * b[14] + a[13] * b[15];
    res[14] = a[2] * b[12] + a[6] * b[13] + a[10] * b[14] + a[14] * b[15];
    res[15] = a[3] * b[12] + a[7] * b[13] + a[11] * b[14] + a[15] * b[15];
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
    ret.x = a.data[0] * v[0] + a.data[4] * v[1] + a.data[8] * v[2] + a.data[12] * v[3];
    ret.y = a.data[1] * v[0] + a.data[5] * v[1] + a.data[9] * v[2] + a.data[13] * v[3];
    ret.z = a.data[2] * v[0] + a.data[6] * v[1] + a.data[10] * v[2] + a.data[14] * v[3];
    ret.w = a.data[3] * v[0] + a.data[7] * v[1] + a.data[11] * v[2] + a.data[15] * v[3];
    return ret;
}
} // namespace bul
