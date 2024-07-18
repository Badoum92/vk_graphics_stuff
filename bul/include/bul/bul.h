#pragma once

#include <stdint.h>
#include <stddef.h>

#define _BUL_CONCAT(a, b) a##b
#define BUL_CONCAT(a, b)  _BUL_CONCAT(a, b)

#if defined(BUL_NO_ASSERT)
#define ASSERT(CONDITION, ...)
#define ENSURE(EXPR) [[maybe_unused]] bool BUL_CONCAT(_, __COUNTER__) = (EXPR)
#else
#define ASSERT(CONDITION, ...) bul::_assert((CONDITION), #CONDITION, __FILE__, __LINE__ __VA_OPT__(,) __VA_ARGS__)
#define ENSURE(EXPR)           bul::_assert((EXPR), #EXPR, __FILE__, __LINE__, nullptr)
#endif

#define BUL_ARRAY_SIZE(ARRAY) (sizeof(ARRAY) / sizeof(*ARRAY))

#define BUL_FORWARD(...) static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)

template <typename F>
struct defer_finalizer
{
    template <typename T>
    defer_finalizer(T&& f_)
        : f(BUL_FORWARD(f_))
    {}

    ~defer_finalizer()
    {
        f();
    }

    F f;
};

struct
{
    template <typename F>
    defer_finalizer<F> operator<<(F&& f)
    {
        return defer_finalizer<F>(BUL_FORWARD(f));
    }
} _deferrer;

#define defer auto BUL_CONCAT(__deferred_lambda_call, __COUNTER__) = _deferrer << [&]

inline constexpr uint32_t operator"" _KB(uint64_t bytes)
{
    return (uint32_t)(bytes << 10);
}

inline constexpr uint32_t operator"" _MB(uint64_t bytes)
{
    return (uint32_t)(bytes << 20);
}

inline constexpr uint32_t operator"" _GB(uint64_t bytes)
{
    return (uint32_t)(bytes << 30);
}

namespace bul
{
void _assert(bool condition, const char* str, const char* file, unsigned line, const char* fmt = nullptr, ...);

template <typename T = void*>
const T ptr_offset(const void* ptr, size_t offset)
{
    return (const T)((const uint8_t*)ptr + offset);
}

template <typename T = void*>
T ptr_offset(void* ptr, size_t offset)
{
    return (T)((uint8_t*)ptr + offset);
}

inline constexpr uint32_t align(uint32_t n, uint32_t alignment)
{
    return (n + alignment - 1) & ~(alignment - 1);
}

inline constexpr uint64_t align(uint64_t n, uint64_t alignment)
{
    return (n + alignment - 1) & ~(alignment - 1);
}

inline void* align(void* ptr, uint64_t alignment)
{
    uint64_t n = (uint64_t)ptr;
    return (void*)align(n, alignment);
}

inline constexpr uint32_t next_pow2(uint32_t n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

inline constexpr uint64_t next_pow2(uint64_t n)
{
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    n++;
    return n;
}
} // namespace bul
