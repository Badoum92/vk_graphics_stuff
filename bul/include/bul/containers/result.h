#pragma once

#include <concepts>

#include "bul/bul.h"

namespace bul
{
template <typename E>
class Error
{
public:
    template <typename... Args>
    requires std::constructible_from<E, Args...>
    Error(Args&&... args)
        : error_{std::forward<Args>(args)...}
    {}

    Error(const E& e)
        : error_{e}
    {}

    Error(E&& e)
        : error_{e}
    {}

    const E& get() const
    {
        return error_;
    }

    E& get()
    {
        return error_;
    }

private:
    E error_;
};

template <typename R, typename E>
class Result
{
static_assert(!std::is_same_v<R, E>);
public:
    template <typename... Args>
    requires std::constructible_from<R, Args...>
    Result(Args&&... args)
        : value_{std::forward<Args>(args)...}
        , has_value_{true}
    {}

    template <typename... Args>
    requires std::constructible_from<E, Args...>
    Result(Args&&... args)
        : error_{std::forward<Args>(args)...}
        , has_value_{false}
    {}

    Result(const R& value)
        : value_{value}
        , has_value_{true}
    {}

    Result(R&& value)
        : value_{value}
        , has_value_{true}
    {}

    Result(const Error<E>& error)
        : error_{error}
        , has_value_{false}
    {}

    Result(Error<E>&& error)
        : error_{error}
        , has_value_{false}
    {}

    ~Result()
    {
        if (has_value_)
        {
            if constexpr (std::is_trivially_destructible_v<R>)
            {
                value_.~R();
            }
        }
        else
        {
            if constexpr (std::is_trivially_destructible_v<E>)
            {
                error_.~Error<E>();
            }
        }
    }

    bool has_value() const
    {
        return has_value_;
    }

    operator bool() const
    {
        return has_value_;
    }

    const R& value() const
    {
        ASSERT(has_value);
        return value;
    }

    R& value()
    {
        ASSERT(has_value);
        return value;
    }

    const E& error() const
    {
        ASSERT(!has_value);
        return error.get();
    }

    E& error()
    {
        ASSERT(!has_value);
        return error.get();
    }

    const R& operator*() const
    {
        ASSERT(has_value);
        return value_;
    }

    R& operator*()
    {
        ASSERT(has_value);
        return value_;
    }

    const R* operator->() const
    {
        ASSERT(has_value);
        return &value_;
    }

    R* operator->()
    {
        ASSERT(has_value);
        return &value_;
    }

private:
    union
    {
        R value_;
        Error<E> error_;
    };
    bool has_value_;
};
} // namespace bul
