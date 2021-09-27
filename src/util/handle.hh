#pragma once

#include <cassert>
#include <numeric>

template <typename T>
class Handle
{
public:
    inline static Handle<T> invalid()
    {
        static Handle<T> invalid_handle{};
        return invalid_handle;
    }

    Handle()
        : value_(UINT32_MAX)
        , id_(UINT32_MAX)
    {}

    explicit Handle(uint32_t value)
        : value_(value)
    {
        static uint32_t id = 0;
        id_ = id++;
        assert(value != UINT32_MAX);
    }

    inline uint32_t value() const
    {
        return value_;
    }

    inline bool is_valid() const
    {
        return value_ != UINT32_MAX && id_ != UINT32_MAX;
    }

    inline bool operator==(const Handle& other) const
    {
        return value_ == other.value_ && id_ == other.id_;
    }

    inline bool operator<(const Handle& other) const
    {
        return value_ < other.value;
    }

private:
    uint32_t value_;
    uint32_t id_;
};
