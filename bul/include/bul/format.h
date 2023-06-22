#pragma once

#include <cstdarg>

namespace bul
{
const char* format(const char* fmt, ...);
const char* format(const char* fmt, va_list args);
} // namespace bul
