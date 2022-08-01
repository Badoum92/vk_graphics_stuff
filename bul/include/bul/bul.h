#pragma once

#include <cstdint>

#define ASSERT(COND)          bul::internal_assert((COND), #COND, nullptr, __FILE__, __LINE__)
#define ASSERT_MSG(COND, MSG) bul::internal_assert((COND), #COND, (MSG), __FILE__, __LINE__)

namespace bul
{
void internal_assert(bool cond, const char* cond_str, const char* msg, const char* file, unsigned line);
} // namespace bul
