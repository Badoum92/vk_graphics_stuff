#pragma once

#include <cstdint>
#include <cstddef>

struct non_copyable
{
    non_copyable() = default;
    non_copyable(const non_copyable&) = delete;
    void operator=(const non_copyable&) = delete;
};

struct non_movable
{
    non_movable() = default;
    non_movable(non_movable&&) = delete;
    non_movable& operator=(non_movable&&) = delete;
};
