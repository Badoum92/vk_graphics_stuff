#include "bul/bul.h"

#include <stdio.h>
#include <stdarg.h>

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


    _log(log_level::error, file, line, "Assertion failed: %s", str);
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        char msg_buf[1024];
        vsnprintf(msg_buf, sizeof(msg_buf), fmt, args);
        _log(log_level::error, file, line, "%s", msg_buf);
        va_end(args);
    }

#if defined(_WIN32)
    char debug_output[1024];
    snprintf(debug_output, sizeof(debug_output), "%s(%d): Assertion failed: %s\n", file, line, str);
    OutputDebugString(debug_output);
#endif

    __debugbreak();
}
} // namespace bul
