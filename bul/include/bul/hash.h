#pragma once

#include <cstdint>
#include <string_view>
#include <concepts>

namespace bul
{
template <typename T>
concept is_container = requires(T value)
{
    { value.data() } -> std::convertible_to<const void*>;
    { value.size() } -> std::convertible_to<size_t>;
};

uint64_t hash(const void* data, size_t size);

template <typename T>
requires(!std::constructible_from<std::string_view, T>)
uint64_t hash(const T& value)
{
    return hash(&value, sizeof(value));
}

template <typename T>
requires(!std::constructible_from<std::string_view, T> && is_container<T>)
uint64_t hash(const T& value)
{
    return hash(value.data(), value.size());
}

template <typename T>
requires(std::constructible_from<std::string_view, T>)
uint64_t hash(const T& value) noexcept
{
    std::string_view sv = value;
    return hash(sv.data(), sv.size());
}
} // namespace bul
