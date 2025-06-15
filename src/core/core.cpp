#include "core/core.h"

#include <stdio.h>
#include <stdarg.h>

#include "core/log.h"

#if defined(_WIN32)
#include <windows.h>
#endif

void _assert(bool condition, const char* str, const char* file, unsigned line, const char* fmt, ...)
{
    if (condition) [[likely]]
    {
        return;
    }

    _log(LOG_LEVEL_ERROR, file, line, "Assertion failed: %s", str);
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        char msg_buf[1024];
        vsnprintf(msg_buf, sizeof(msg_buf), fmt, args);
        _log(LOG_LEVEL_ERROR, file, line, "%s", msg_buf);
        va_end(args);
    }

#if defined(_WIN32)
    char debug_output[1024];
    snprintf(debug_output, sizeof(debug_output), "%s(%d): Assertion failed: %s\n", file, line, str);
    OutputDebugStringA(debug_output);
#endif

    __debugbreak();
}
