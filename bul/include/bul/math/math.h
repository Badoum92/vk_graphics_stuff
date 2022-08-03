#pragma once

namespace bul
{
template <typename T>
constexpr T radians(T deg)
{
    return deg * static_cast<T>(0.01745329251994329576923690768489);
}

template <typename T>
constexpr T degrees(T rad)
{
    return rad * static_cast<T>(57.295779513082320876798154814105);
}
} // namespace bul
