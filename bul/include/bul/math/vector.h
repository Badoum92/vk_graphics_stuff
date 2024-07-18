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
    T x = 0;
    T y = 0;

    constexpr vec2() = default;

    constexpr explicit vec2(T val)
        : x(val)
        , y(val)
    {}

    constexpr vec2(T x_, T y_)
        : x(x_)
        , y(y_)
    {}

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

    constexpr bool operator==(const vec2& other) const = default;

    constexpr T* data()
    {
        return &x;
    }

    constexpr const T* data() const
    {
        return &x;
    }

    T& operator[](uint32_t i)
    {
        ASSERT(i < SIZE);
        return data()[i];
    }

    const T& operator[](uint32_t i) const
    {
        ASSERT(i < SIZE);
        return data()[i];
    }
};

template <typename T>
struct vec3
{
    using value_t = T;

    static constexpr uint32_t SIZE = 3;
    T x = 0;
    T y = 0;
    T z = 0;

    constexpr vec3() = default;

    constexpr explicit vec3(T val)
        : x(val)
        , y(val)
        , z(val)
    {}

    constexpr vec3(T x_, T y_, T z_)
        : x(x_)
        , y(y_)
        , z(z_)
    {}

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

    constexpr bool operator==(const vec3& other) const = default;

    constexpr T* data()
    {
        return &x;
    }

    constexpr const T* data() const
    {
        return &x;
    }

    T& operator[](uint32_t i)
    {
        ASSERT(i < SIZE);
        return data()[i];
    }

    const T& operator[](uint32_t i) const
    {
        ASSERT(i < SIZE);
        return data()[i];
    }
};

template <typename T>
struct vec4
{
    using value_t = T;

    static constexpr uint32_t SIZE = 4;
    T x = 0;
    T y = 0;
    T z = 0;
    T w = 0;

    constexpr vec4() = default;

    constexpr explicit vec4(T val)
        : x(val)
        , y(val)
        , z(val)
        , w(val)
    {}

    constexpr vec4(T x_, T y_, T z_, T w_)
        : x(x_)
        , y(y_)
        , z(z_)
        , w(w_)
    {}

    constexpr vec4(const vec3<T>& xyz, T w_)
        : x(xyz.x)
        , y(xyz.y)
        , z(xyz.z)
        , w(w_)
    {}

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

    constexpr bool operator==(const vec4& other) const = default;

    constexpr T* data()
    {
        return &x;
    }

    constexpr const T* data() const
    {
        return &x;
    }

    T& operator[](uint32_t i)
    {
        ASSERT(i < SIZE);
        return data()[i];
    }

    const T& operator[](uint32_t i) const
    {
        ASSERT(i < SIZE);
        return data()[i];
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

template <typename V>
auto min(const V& v)
{
    auto res = v[0];
    for (uint32_t i = 1; i < V::SIZE; ++i)
    {
        res = res < v[i] ? res : v[i];
    }
    return res;
}

template <typename V>
auto max(const V& v)
{
    auto res = v[0];
    for (uint32_t i = 1; i < V::SIZE; ++i)
    {
        res = res > v[i] ? res : v[i];
    }
    return res;
}

template <typename V>
uint32_t min_comp(const V& v)
{
    uint32_t comp = 0;
    for (uint32_t i = 1; i < V::SIZE; ++i)
    {
        if (v[i] < v[comp])
        {
            comp = i;
        }
    }
    return comp;
}

template <typename V>
uint32_t max_comp(const V& v)
{
    uint32_t comp = 0;
    for (uint32_t i = 1; i < V::SIZE; ++i)
    {
        if (v[i] > v[comp])
        {
            comp = i;
        }
    }
    return comp;
}

template <typename V>
auto dot(const V& a, const V& b)
{
    typename V::value_t res = 0;
    for (uint32_t i = 0; i < V::SIZE; ++i)
    {
        res += a[i] * b[i];
    }
    return res;
}

template <typename V>
auto length(const V& v)
{
    return sqrtf((float)dot(v, v));
}

template <typename V>
auto distance(const V& a, const V& b)
{
    return length(a - b);
}

template <typename V>
V normalize(const V& v)
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

inline vec3f cross(const vec3f& a, const vec3f& b)
{
    return {a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y};
}

inline constexpr vec3f RIGHT = {1, 0, 0};
inline constexpr vec3f UP = {0, 1, 0};
inline constexpr vec3f FRONT = {0, 0, -1};
} // namespace bul
