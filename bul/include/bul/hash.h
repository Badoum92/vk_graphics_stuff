#pragma once

#include <cstdint>
#include <string_view>
#include <concepts>

namespace bul
{
uint64_t hash(const void* data, size_t size);

template <typename T>
requires(!std::constructible_from<std::string_view, T>)
uint64_t hash(const T& value)
{
    return hash(&value, sizeof(value));
}

template <typename T>
requires(std::constructible_from<std::string_view, T>)
uint64_t hash(const T& value) noexcept
{
    std::string_view sv = value;
    return hash(sv.data(), sv.size());
}

template <typename T>
uint64_t hash_combine(uint64_t seed, const T& v)
{
    seed ^= hash(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
}

template <typename T, typename... Rest>
uint64_t hash_combine(uint64_t seed, const T& v, Rest... rest)
{
    seed = hash_combine(v);
    hash_combine(seed, rest...);
    return seed;
}
} // namespace bul
