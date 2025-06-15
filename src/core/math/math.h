#pragma once

constexpr float math_pi = 3.1415926535897932384626433832795f;
constexpr float math_two_pi = 2.0f * math_pi;
constexpr float math_half_pi = 0.5f * math_pi;
constexpr float math_epsilon = 0.00001f;

#define math_radians(deg) (deg * 0.01745329251994329576923690768489f)
#define math_degrees(rad) (rad * 57.295779513082320876798154814105f)

#define math_min(a, b)              ((a) < (b) ? (a) : (b))
#define math_max(a, b)              ((a) > (b) ? (a) : (b))
#define math_clamp(min, value, max) ((value) <= (min) ? (min) : (value) >= (max) ? (max) : (value))
