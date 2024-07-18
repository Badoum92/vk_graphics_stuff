#include "bul/log.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#if defined(_WIN32)
#include <windows.h>
#endif

#include "bul/containers/enum_array.h"

namespace bul
{
static constexpr enum_array<log_level, const char*> levels = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
};

static constexpr enum_array<log_level, const char*> colors = {
    "\x1b[36m",
    "\x1b[32m",
    "\x1b[33m",
    "\x1b[31m",
};

void _log(log_level level, const char* file, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char time_buf[16];
    time_t t = time(NULL);
    time_buf[strftime(time_buf, sizeof(time_buf), "%H:%M:%S", localtime(&t))] = 0;

    printf("%s %s%-7s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", time_buf, colors[level], levels[level], file, line);
    vprintf(fmt, args);
    printf("\n");
    fflush(stdout);

    va_end(args);
}
} // namespace bul
