#include "bul/math/vector.h"

#include <cmath>

namespace bul
{
template <typename V>
static auto min_(V v)
{
    auto res = v[0];
    for (size_t i = 1; i < V::SIZE; ++i)
    {
        res = std::min(res, v[i]);
    }
    return res;
}

template <typename V>
static auto max_(V v)
{
    auto res = v[0];
    for (size_t i = 1; i < V::SIZE; ++i)
    {
        res = std::max(res, v[i]);
    }
    return res;
}

template <typename V>
static size_t min_comp_(V v)
{
    size_t comp = 0;
    for (size_t i = 1; i < V::SIZE; ++i)
    {
        if (v[i] < v[comp])
        {
            comp = i;
        }
    }
    return comp;
}

template <typename V>
static size_t max_comp_(V v)
{
    size_t comp = 0;
    for (size_t i = 1; i < V::SIZE; ++i)
    {
        if (v[i] > v[comp])
        {
            comp = i;
        }
    }
    return comp;
}

template <typename V>
static auto dot_(V a, V b)
{
    typename V::value_t res = 0;
    for (size_t i = 0; i < V::SIZE; ++i)
    {
        res += a[i] * b[i];
    }
    return res;
}

template <typename V>
static auto length_(V v)
{
    return std::sqrt(dot(v, v));
}

template <typename V>
static auto distance_(V a, V b)
{
    return length(a - b);
}

template <typename V>
static V normalize_(V v)
{
    return v / length(v);
}

float min(vec2f v)
{
    return min_(v);
}
float min(vec3f v)
{
    return min_(v);
}
float min(vec4f v)
{
    return min_(v);
}

float max(vec2f v)
{
    return max_(v);
}
float max(vec3f v)
{
    return max_(v);
}
float max(vec4f v)
{
    return max_(v);
}

size_t min_comp(vec2f v)
{
    return min_comp_(v);
}
size_t min_comp(vec3f v)
{
    return min_comp_(v);
}
size_t min_comp(vec4f v)
{
    return min_comp_(v);
}

size_t max_comp(vec2f v)
{
    return max_comp_(v);
}
size_t max_comp(vec3f v)
{
    return max_comp_(v);
}
size_t max_comp(vec4f v)
{
    return max_comp_(v);
}

float dot(vec2f a, vec2f b)
{
    return dot_(a, b);
}
float dot(vec3f a, vec3f b)
{
    return dot_(a, b);
}
float dot(vec4f a, vec4f b)
{
    return dot_(a, b);
}

float length(vec2f v)
{
    return length_(v);
}
float length(vec3f v)
{
    return length_(v);
}
float length(vec4f v)
{
    return length_(v);
}

float distance(vec2f a, vec2f b)
{
    return distance_(a, b);
}
float distance(vec3f a, vec3f b)
{
    return distance_(a, b);
}
float distance(vec4f a, vec4f b)
{
    return distance_(a, b);
}

vec2f normalize(vec2f v)
{
    return normalize_(v);
}
vec3f normalize(vec3f v)
{
    return normalize_(v);
}
vec4f normalize(vec4f v)
{
    return normalize_(v);
}

vec3f cross(vec3f a, vec3f b)
{
    return {a.y * b.z - b.y * a.z, a.z * b.x - b.z * a.x, a.x * b.y - b.x * a.y};
}
} // namespace bul
