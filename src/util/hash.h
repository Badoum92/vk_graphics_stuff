#pragma once

#include <functional>
#include <vector>

template<typename T>
size_t hash_value(const T& v)
{
    return std::hash<T>{}(v);
}

inline void hash_combine(std::size_t&)
{}

template <typename T, typename... Rest>
inline void hash_combine(std::size_t& seed, const T& v, Rest... rest)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, rest...);
}

namespace std
{
template <typename T>
struct hash<std::vector<T>>
{
    size_t operator()(const std::vector<T>& v) const noexcept
    {
        std::size_t hash = v.size();
        for (size_t i = 0; i < v.size(); ++i)
        {
            hash_combine(hash, v[i]);
        }
        return hash;
    }
};
} // namespace std
