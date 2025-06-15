#pragma once

#include <stdint.h>
#include <stddef.h>

#define _MACRO_CONCAT(a, b) a##b
#define MACRO_CONCAT(a, b)  _MACRO_CONCAT(a, b)

#if defined(NO_ASSERT)
#define ASSERT(CONDITION, ...)
#define ENSURE(EXPR) [[maybe_unused]] bool BUL_CONCAT(_, __COUNTER__) = (EXPR)
#else
#define ASSERT(CONDITION, ...) _assert((CONDITION), #CONDITION, __FILE__, __LINE__, ##__VA_ARGS__)
#define ENSURE(EXPR)           _assert((EXPR), #EXPR, __FILE__, __LINE__, nullptr)
#endif

#define ARRAY_SIZE(ARRAY) (sizeof(ARRAY) / sizeof(*ARRAY))

#define KB(BYTES) ((BYTES) << 10)
#define MB(BYTES) ((BYTES) << 20)
#define GB(BYTES) ((BYTES) << 30)

void _assert(bool condition, const char* str, const char* file, unsigned line, const char* fmt = nullptr, ...);

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

inline void* align(void* ptr, uint32_t alignment)
{
    uint64_t n = (uint64_t)ptr;
    return (void*)align(n, (uint64_t)alignment);
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
