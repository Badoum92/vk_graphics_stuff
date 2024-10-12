#pragma once

#include "bul/bul.h"

#include <math.h>

namespace bul
{
template <typename T>
struct vec2
{
    using value_t = T;

    static constexpr uint32_t SIZE = 2;
    T x;
    T y;

    constexpr void operator*=(T val)
    {
        x *= val;
        y *= val;
    }

    constexpr void operator/=(T val)
    {
        x /= val;
        y /= val;
    }

    constexpr void operator+=(const vec2& other)
    {
        x += other.x;
        y += other.y;
    }

    constexpr void operator-=(const vec2& other)
    {
        x -= other.x;
        y -= other.y;
    }

    constexpr void operator*=(const vec2& other)
    {
        x *= other.x;
        y *= other.y;
    }

    constexpr void operator/=(const vec2& other)
    {
        x /= other.x;
        y /= other.y;
    }

    constexpr T& operator[](uint32_t i)
    {
        return (&x)[i];
    }

    constexpr const T& operator[](uint32_t i) const
    {
        return (&x)[i];
    }
};

template <typename T>
struct vec3
{
    using value_t = T;

    static constexpr uint32_t SIZE = 3;
    T x;
    T y;
    T z;

    constexpr void operator*=(T val)
    {
        x *= val;
        y *= val;
        z *= val;
    }

    constexpr void operator/=(T val)
    {
        x /= val;
        y /= val;
        z /= val;
    }

    constexpr void operator+=(const vec3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    constexpr void operator-=(const vec3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    constexpr void operator*=(const vec3& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
    }

    constexpr void operator/=(const vec3& other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
    }

    constexpr T& operator[](uint32_t i)
    {
        return (&x)[i];
    }

    constexpr const T& operator[](uint32_t i) const
    {
        return (&x)[i];
    }
};

template <typename T>
struct vec4
{
    using value_t = T;

    static constexpr uint32_t SIZE = 4;
    T x;
    T y;
    T z;
    T w;

    constexpr operator vec3<T>() const
    {
        return vec3<T>{x, y, z};
    }

    constexpr void operator*=(T val)
    {
        x *= val;
        y *= val;
        z *= val;
        w *= val;
    }

    constexpr void operator/=(T val)
    {
        x /= val;
        y /= val;
        z /= val;
        w /= val;
    }

    constexpr void operator+=(const vec4& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
    }

    constexpr void operator-=(const vec4& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
    }

    constexpr void operator*=(const vec4& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
    }

    constexpr void operator/=(const vec4& other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
        w /= other.w;
    }

    constexpr T& operator[](uint32_t i)
    {
        return (&x)[i];
    }

    constexpr const T& operator[](uint32_t i) const
    {
        return (&x)[i];
    }
};

template <typename T>
constexpr vec2<T> operator*(const vec2<T>& v, T val)
{
    return {v.x * val, v.y * val};
}

template <typename T>
constexpr vec2<T> operator/(const vec2<T>& v, T val)
{
    return {v.x / val, v.y / val};
}

template <typename T>
constexpr vec2<T> operator+(const vec2<T>& a, const vec2<T>& b)
{
    return {a.x + b.x, a.y + b.y};
}

template <typename T>
constexpr vec2<T> operator-(const vec2<T>& a, const vec2<T>& b)
{
    return {a.x - b.x, a.y - b.y};
}

template <typename T>
constexpr vec2<T> operator*(const vec2<T>& a, const vec2<T>& b)
{
    return {a.x * b.x, a.y * b.y};
}

template <typename T>
constexpr vec2<T> operator/(const vec2<T>& a, const vec2<T>& b)
{
    return {a.x / b.x, a.y / b.y};
}

template <typename T>
constexpr vec3<T> operator*(const vec3<T>& v, T val)
{
    return {v.x * val, v.y * val, v.z * val};
}

template <typename T>
constexpr vec3<T> operator/(const vec3<T>& v, T val)
{
    return {v.x / val, v.y / val, v.z / val};
}

template <typename T>
constexpr vec3<T> operator+(const vec3<T>& a, const vec3<T>& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}

template <typename T>
constexpr vec3<T> operator-(const vec3<T>& a, const vec3<T>& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

template <typename T>
constexpr vec3<T> operator*(const vec3<T>& a, const vec3<T>& b)
{
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}

template <typename T>
constexpr vec3<T> operator/(const vec3<T>& a, const vec3<T>& b)
{
    return {a.x / b.x, a.y / b.y, a.z / b.z};
}

template <typename T>
constexpr vec4<T> operator*(const vec4<T>& v, T val)
{
    return {v.x * val, v.y * val, v.z * val, v.w * val};
}

template <typename T>
constexpr vec4<T> operator/(const vec4<T>& v, T val)
{
    return {v.x / val, v.y / val, v.z / val, v.w / val};
}

template <typename T>
constexpr vec4<T> operator+(const vec4<T>& a, const vec4<T>& b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}

template <typename T>
constexpr vec4<T> operator-(const vec4<T>& a, const vec4<T>& b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}

template <typename T>
constexpr vec4<T> operator*(const vec4<T>& a, const vec4<T>& b)
{
    return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}

template <typename T>
constexpr vec4<T> operator/(const vec4<T>& a, const vec4<T>& b)
{
    return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
}

template <typename T>
constexpr bool operator==(const vec2<T>& a, const vec2<T>& b)
{
    return a.x == b.x && a.y == b.y;
}

template <typename T>
constexpr bool operator==(const vec3<T>& a, const vec3<T>& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

template <typename T>
constexpr bool operator==(const vec4<T>& a, const vec4<T>& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

template <typename V>
constexpr auto min(const V& v)
{
    auto res = v[0];
    for (uint32_t i = 1; i < V::SIZE; ++i)
    {
        res = res < v[i] ? res : v[i];
    }
    return res;
}

template <typename V>
constexpr auto max(const V& v)
{
    auto res = v[0];
    for (uint32_t i = 1; i < V::SIZE; ++i)
    {
        res = res > v[i] ? res : v[i];
    }
    return res;
}

template <typename V>
constexpr uint32_t min_index(const V& v)
{
    uint32_t index = 0;
    for (uint32_t i = 1; i < V::SIZE; ++i)
    {
        if (v[i] < v[index])
        {
            index = i;
        }
    }
    return index;
}

template <typename V>
constexpr uint32_t max_index(const V& v)
{
    uint32_t index = 0;
    for (uint32_t i = 1; i < V::SIZE; ++i)
    {
        if (v[i] > v[index])
        {
            index = i;
        }
    }
    return index;
}

template <typename V>
constexpr auto dot(const V& a, const V& b)
{
    typename V::value_t res = 0;
    for (uint32_t i = 0; i < V::SIZE; ++i)
    {
        res += a[i] * b[i];
    }
    return res;
}

template <typename V>
constexpr auto length(const V& v)
{
    return sqrtf((float)dot(v, v));
}

template <typename V>
constexpr auto distance(const V& a, const V& b)
{
    return length(a - b);
}

template <typename V>
constexpr V normalize(const V& v)
{
    return v / length(v);
}

using vec2f = vec2<float>;
using vec2i = vec2<int32_t>;
using vec2u = vec2<uint32_t>;

using vec3f = vec3<float>;
using vec3i = vec3<int32_t>;
using vec3u = vec3<uint32_t>;

using vec4f = vec4<float>;
using vec4i = vec4<int32_t>;
using vec4u = vec4<uint32_t>;

constexpr inline vec3f cross(const vec3f& a, const vec3f& b)
{
    return {a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y};
}

inline constexpr vec3f RIGHT = {1, 0, 0};
inline constexpr vec3f UP = {0, 1, 0};
inline constexpr vec3f FRONT = {0, 0, -1};
} // namespace bul
