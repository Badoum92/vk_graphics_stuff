#pragma once

constexpr float math_pi = 3.1415926535897932384626433832795f;
constexpr float math_two_pi = 2.0f * math_pi;
constexpr float math_half_pi = 0.5f * math_pi;
constexpr float math_epsilon = 0.00001f;

#define RADIANS(deg) (deg * 0.01745329251994329576923690768489f)
#define DEGREES(rad) (rad * 57.295779513082320876798154814105f)

#define MIN(a, b)              ((a) < (b) ? (a) : (b))
#define MAX(a, b)              ((a) > (b) ? (a) : (b))
#define CLAMP(value, min, max) ((value) <= (min) ? (min) : (value) >= (max) ? (max) : (value))

#define LERP(a, b, t) ((a) * (1.0f - (t)) + (b) * (t))