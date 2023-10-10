#include "bul/bul.h"

#include <cstdio>
#include <cstdarg>

#include "bul/format.h"
#include "bul/log.h"

#if defined(_WIN32)
#include <windows.h>
#endif

namespace bul
{
void _assert(bool condition, const char* str, const char* file, unsigned line, const char* fmt, ...)
{
    if (condition) [[likely]]
    {
        return;
    }

    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        const char* msg = format(fmt, args);
        printf("Assertion (%s) failed in %s line %u: %s\n", str, file, line, msg);
        va_end(args);
    }
    else
    {
        printf("Assertion (%s) failed in %s line %u\n", str, file, line);
    }

#if defined(_WIN32)
    const char* debug_output = format("%s(%d): assertion (%s) failed\n", file, line, str);
    OutputDebugString(debug_output);
#endif

    __debugbreak();
}
} // namespace bul
