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

    explicit constexpr Handle(uint32_t val, uint32_t ver = 0)
        : value(val)
        , version(ver)
    {
        ASSERT(val != UINT32_MAX);
    }

    inline bool is_valid() const
    {
        return value != UINT32_MAX;
    }

    inline bool operator==(const Handle& other) const
    {
        return value == other.value && version == other.version;
    }

    inline bool operator<(const Handle& other) const
    {
        return value < other.value;
    }

    explicit operator bool() const
    {
        return is_valid();
    }

    uint32_t value = 0;
    uint32_t version = 0;

    inline static const Handle<T> invalid;
};
} // namespace bul
