#pragma once

#include "bul/bul.h"

namespace bul
{
template <typename T>
struct handle
{
    constexpr bool is_valid() const
    {
        return value != UINT32_MAX;
    }

    constexpr bool operator==(const handle& other) const
    {
        return value == other.value && version == other.version;
    }

    constexpr bool operator<(const handle& other) const
    {
        return value < other.value;
    }

    constexpr operator bool() const
    {
        return is_valid();
    }

    static constexpr handle<T> invalid()
    {
        return {UINT32_MAX, UINT32_MAX};
    }

    uint32_t value = UINT32_MAX;
    uint32_t version = UINT32_MAX;
};
} // namespace bul
