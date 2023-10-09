#pragma once

#include "bul/bul.h"

namespace bul
{
template <typename T>
struct Handle
{
    Handle()
        : value(UINT32_MAX)
        , version(UINT32_MAX)
    {}

    explicit constexpr Handle(uint32_t value_, uint32_t version_ = 0)
        : value(value_)
        , version(version_)
    {
        ASSERT(value != UINT32_MAX && version != UINT32_MAX);
    }

    constexpr bool is_valid() const
    {
        return value != UINT32_MAX;
    }

    constexpr bool operator==(const Handle& other) const
    {
        return value == other.value && version == other.version;
    }

    constexpr bool operator<(const Handle& other) const
    {
        return value < other.value;
    }

    constexpr operator bool() const
    {
        return is_valid();
    }

    uint32_t value = 0;
    uint32_t version = 0;
};
} // namespace bul
