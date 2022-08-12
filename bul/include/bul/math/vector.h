#pragma once

#include "bul/bul.h"

namespace bul
{
template <typename T>
struct vec2
{
    using value_t = T;

    static constexpr size_t SIZE = 2;
    T x = 0;
    T y = 0;

    constexpr vec2() = default;

    constexpr vec2(T val)
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

    constexpr void operator+=(vec2 other)
    {
        x += other.x;
        y += other.y;
    }
    constexpr void operator-=(vec2 other)
    {
        x -= other.x;
        y -= other.y;
    }
    constexpr void operator*=(vec2 other)
    {
        x *= other.x;
        y *= other.y;
    }
    constexpr void operator/=(vec2 other)
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

    T& operator[](size_t i)
    {
        return data()[i];
    }
    const T& operator[](size_t i) const
    {
        return data()[i];
    }
};

template <typename T>
struct vec3
{
    using value_t = T;

    static constexpr size_t SIZE = 3;
    T x = 0;
    T y = 0;
    T z = 0;

    constexpr vec3() = default;

    constexpr vec3(T val)
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

    constexpr void operator+=(vec3 other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
    }
    constexpr void operator-=(vec3 other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }
    constexpr void operator*=(vec3 other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
    }
    constexpr void operator/=(vec3 other)
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

    T& operator[](size_t i)
    {
        return data()[i];
    }
    const T& operator[](size_t i) const
    {
        return data()[i];
    }
};

template <typename T>
struct vec4
{
    using value_t = T;

    static constexpr size_t SIZE = 4;
    T x = 0;
    T y = 0;
    T z = 0;
    T w = 0;

    constexpr vec4() = default;

    constexpr vec4(T val)
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

    constexpr void operator+=(vec4 other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        w += other.w;
    }
    constexpr void operator-=(vec4 other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        w -= other.w;
    }
    constexpr void operator*=(vec4 other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
        w *= other.w;
    }
    constexpr void operator/=(vec4 other)
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

    T& operator[](size_t i)
    {
        return data()[i];
    }
    const T& operator[](size_t i) const
    {
        return data()[i];
    }
};

template <typename T>
constexpr vec2<T> operator*(vec2<T> v, T val)
{
    return {v.x * val, v.y * val};
}
template <typename T>
constexpr vec2<T> operator/(vec2<T> v, T val)
{
    return {v.x / val, v.y / val};
}
template <typename T>
constexpr vec2<T> operator+(vec2<T> a, vec2<T> b)
{
    return {a.x + b.x, a.y + b.y};
}
template <typename T>
constexpr vec2<T> operator-(vec2<T> a, vec2<T> b)
{
    return {a.x - b.x, a.y - b.y};
}
template <typename T>
constexpr vec2<T> operator*(vec2<T> a, vec2<T> b)
{
    return {a.x * b.x, a.y * b.y};
}
template <typename T>
constexpr vec2<T> operator/(vec2<T> a, vec2<T> b)
{
    return {a.x / b.x, a.y / b.y};
}

template <typename T>
constexpr vec3<T> operator*(vec3<T> v, T val)
{
    return {v.x * val, v.y * val, v.z * val};
}
template <typename T>
constexpr vec3<T> operator/(vec3<T> v, T val)
{
    return {v.x / val, v.y / val, v.z / val};
}
template <typename T>
constexpr vec3<T> operator+(vec3<T> a, vec3<T> b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z};
}
template <typename T>
constexpr vec3<T> operator-(vec3<T> a, vec3<T> b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
template <typename T>
constexpr vec3<T> operator*(vec3<T> a, vec3<T> b)
{
    return {a.x * b.x, a.y * b.y, a.z * b.z};
}
template <typename T>
constexpr vec3<T> operator/(vec3<T> a, vec3<T> b)
{
    return {a.x / b.x, a.y / b.y, a.z / b.z};
}

template <typename T>
constexpr vec4<T> operator*(vec4<T> v, T val)
{
    return {v.x * val, v.y * val, v.z * val, v.w * val};
}
template <typename T>
constexpr vec4<T> operator/(vec4<T> v, T val)
{
    return {v.x / val, v.y / val, v.z / val, v.w / val};
}
template <typename T>
constexpr vec4<T> operator+(vec4<T> a, vec4<T> b)
{
    return {a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w};
}
template <typename T>
constexpr vec4<T> operator-(vec4<T> a, vec4<T> b)
{
    return {a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w};
}
template <typename T>
constexpr vec4<T> operator*(vec4<T> a, vec4<T> b)
{
    return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
}
template <typename T>
constexpr vec4<T> operator/(vec4<T> a, vec4<T> b)
{
    return {a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w};
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

float min(vec2f v);
float min(vec3f v);
float min(vec4f v);

float max(vec2f v);
float max(vec3f v);
float max(vec4f v);

size_t min_comp(vec2f v);
size_t min_comp(vec3f v);
size_t min_comp(vec4f v);

size_t max_comp(vec2f v);
size_t max_comp(vec3f v);
size_t max_comp(vec4f v);

float dot(vec2f a, vec2f b);
float dot(vec3f a, vec3f b);
float dot(vec4f a, vec4f b);

float length(vec2f v);
float length(vec3f v);
float length(vec4f v);

float distance(vec2f a, vec2f b);
float distance(vec3f a, vec3f b);
float distance(vec4f a, vec4f b);

vec2f normalize(vec2f v);
vec3f normalize(vec3f v);
vec4f normalize(vec4f v);

vec3f cross(vec3f a, vec3f b);

inline constexpr vec3f right = {1, 0, 0};
inline constexpr vec3f up = {0, 1, 0};
inline constexpr vec3f front = {0, 0, -1};
} // namespace bul
