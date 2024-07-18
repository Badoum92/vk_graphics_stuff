#pragma once

#include <float.h>

namespace bul
{
constexpr float pi = 3.1415926535897932384626433832795f;
constexpr float two_pi = 2.0f * pi;
constexpr float half_pi = 0.5f * pi;
constexpr float epsilon = FLT_EPSILON;
} // namespace bul

#define bul_radians(deg) (deg * 0.01745329251994329576923690768489f)
#define bul_degrees(rad) (rad * 57.295779513082320876798154814105f)

#define bul_min(a, b)              ((a) < (b) ? (a) : (b))
#define bul_max(a, b)              ((a) > (b) ? (a) : (b))
#define bul_clamp(min, value, max) ((value) <= (min) ? (min) : (value) >= (max) ? (max) : (value))
