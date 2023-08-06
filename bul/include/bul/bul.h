#pragma once

#include "bul/types.h"

#define _BUL_CONCAT(a, b) a##b
#define BUL_CONCAT(a, b) _BUL_CONCAT(a, b)

#if defined(_DEBUG)
#define ASSERT(COND)          bul::_assert((COND), #COND, nullptr, __FILE__, __LINE__)
#define ASSERT_MSG(COND, MSG) bul::_assert((COND), #COND, (MSG), __FILE__, __LINE__)
#define ENSURE(EXPR)          bul::_assert((EXPR), #EXPR, nullptr, __FILE__, __LINE__)
#else
#define ASSERT(COND)
#define ASSERT_MSG(COND, MSG)
#define ENSURE(EXPR) [[maybe_unused]] bool BUL_CONCAT(_, __COUNTER__) = (EXPR)
#endif

#define BUL_ARRAY_SIZE(ARRAY) (sizeof(ARRAY) / sizeof(*ARRAY))

inline constexpr uint64_t operator"" _KB(uint64_t bytes)
{
    return bytes << 10;
}

inline constexpr uint64_t operator"" _MB(uint64_t bytes)
{
    return bytes << 20;
}

inline constexpr uint64_t operator"" _GB(uint64_t bytes)
{
    return bytes << 30;
}

inline constexpr uint64_t operator"" _TB(uint64_t bytes)
{
    return bytes << 40;
}

namespace bul
{
void _assert(bool cond, const char* cond_str, const char* msg, const char* file, unsigned line);

template <typename Dst = void*>
const Dst ptr_offset(const void* ptr, size_t offset)
{
    return (const Dst)((const uint8_t*)ptr + offset);
}

template <typename Dst = void*>
Dst ptr_offset(void* ptr, size_t offset)
{
    return (Dst)((uint8_t*)ptr + offset);
}

template <typename T>
T align_pow2(T n, T alignment)
{
    return (n + alignment - 1) & -alignment;
}
} // namespace bul
