#pragma once

#include "bul/bul.h"

namespace bul
{
template <typename T>
struct Handle
{
    inline static Handle<T> invalid()
    {
        static Handle<T> invalid_handle = {};
        return invalid_handle;
    }

    Handle()
        : value(UINT32_MAX)
        , version(UINT32_MAX)
    {}

    explicit Handle(uint32_t val, uint32_t ver = 0)
        : value(val)
        , version(ver)
    {
        ASSERT(val != UINT32_MAX);
    }

    inline Handle(const Handle<T>&) = default;
    inline Handle<T>& operator=(const Handle<T>&) = default;
    inline Handle(Handle<T>&&) = default;
    inline Handle<T>& operator=(Handle<T>&&) = default;

    inline bool is_valid() const
    {
        return value != UINT32_MAX && version != UINT32_MAX;
    }

    inline bool operator==(const Handle& other) const
    {
        return value == other.value && version == other.version;
    }

    inline bool operator<(const Handle& other) const
    {
        return value < other.value;
    }

    uint32_t value = 0;
    uint32_t version = 0;
};
} // namespace bul
