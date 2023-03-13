#pragma once

#include <cstdint>

#if defined(_DEBUG)
#define ASSERT(COND)          bul::_assert((COND), #COND, nullptr, __FILE__, __LINE__)
#define ASSERT_MSG(COND, MSG) bul::_assert((COND), #COND, (MSG), __FILE__, __LINE__)
#define ENSURE(EXPR)          bul::_assert((EXPR), #EXPR, nullptr, __FILE__, __LINE__)
#else
#define ASSERT(COND)
#define ASSERT_MSG(COND, MSG)
#define ENSURE(EXPR) (EXPR)
#endif

inline uint64_t operator"" _KB(uint64_t bytes)
{
    return bytes << 10;
}

inline uint64_t operator"" _MB(uint64_t bytes)
{
    return bytes << 20;
}

inline uint64_t operator"" _GB(uint64_t bytes)
{
    return bytes << 30;
}

inline uint64_t operator"" _TB(uint64_t bytes)
{
    return bytes << 40;
}

namespace bul
{
void _assert(bool cond, const char* cond_str, const char* msg, const char* file, unsigned line);
} // namespace bul
