#pragma once

#include <utility>

struct test_struct
{
    enum class state
    {
        default_constructed,
        user_constructed,
        copy_constructed,
        move_constructed,
        moved_from,
        destroyed,
    };

    test_struct()
        : value{0}
        , state{state::default_constructed}
    {}

    test_struct(int val)
        : value{val}
        , state{state::user_constructed}
    {}

    ~test_struct()
    {
        state = state::destroyed;
    }

    test_struct(const test_struct& other)
    {
        *this = other;
    }

    test_struct& operator=(const test_struct& other)
    {
        value = other.value;
        state = state::copy_constructed;
        return *this;
    }

    test_struct(test_struct&& other) noexcept
    {
        *this = std::move(other);
    }

    test_struct& operator=(test_struct&& other) noexcept
    {
        value = other.value;
        state = state::move_constructed;
        other.state = state::moved_from;
        return *this;
    }

    bool default_constructed() const
    {
        return state == state::default_constructed;
    }

    bool user_constructed() const
    {
        return state == state::user_constructed;
    }

    bool copy_constructed() const
    {
        return state == state::copy_constructed;
    }

    bool move_constructed() const
    {
        return state == state::move_constructed;
    }

    bool moved_from() const
    {
        return state == state::moved_from;
    }

    bool destroyed() const
    {
        return state == state::destroyed;
    }

    operator int() const
    {
        return value;
    }

    int value = 0;
    state state = state::default_constructed;
};
