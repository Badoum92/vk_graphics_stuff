#include "bul/log.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>

#if defined(_WIN32)
#include <windows.h>
#endif

#include "bul/containers/enum_array.h"
#include "bul/containers/static_vector.h"

namespace bul
{
static bul::static_vector<log_function_t, 2> log_functions;

static constexpr const char* log_level_str[_log_level_count] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
};

static constexpr const char* log_level_colors[_log_level_count] = {
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

    for (log_function_t log_function : log_functions)
    {
        va_list args_copy;
        va_copy(args_copy, args);
        log_function(level, time_buf, file, line, fmt, args_copy);
    }

    printf("%s %s%-7s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", time_buf, log_level_colors[level], log_level_str[level], file,
           line);
    vprintf(fmt, args);
    printf("\n");
    fflush(stdout);

    va_end(args);
}

void add_log_function(log_function_t function)
{
    log_functions.push_back(function);
}

void remove_log_function(log_function_t function)
{
    uint32_t index = log_functions.find(function);
    if (index != UINT32_MAX)
    {
        log_functions.swap_remove(index);
    }
}
} // namespace bul
