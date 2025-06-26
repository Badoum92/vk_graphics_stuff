#include "core/log.h"
#include "core/core.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#if defined(_WIN32)
#include <windows.h>
#endif

static constexpr uint32_t max_log_functions = 2;
static uint32_t num_log_functions = 0;
static log_function_t log_functions[max_log_functions];
static char* log_buffer = nullptr;
static uint32_t log_buffer_size = 0;

static constexpr const char* log_level_str[LOG_LEVEL_COUNT] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
};

static constexpr const char* log_level_colors[LOG_LEVEL_COUNT] = {
    "\x1b[36m",
    "\x1b[32m",
    "\x1b[33m",
    "\x1b[31m",
};

void _log(LOG_LEVEL level, const char* file, int line, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char time_buf[16];
    time_t t = time(NULL);
    time_buf[strftime(time_buf, sizeof(time_buf), "%H:%M:%S", localtime(&t))] = 0;

    for (uint32_t i = 0; i < num_log_functions; ++i)
    {
        va_list args_copy;
        va_copy(args_copy, args);
        log_functions[i](level, time_buf, file, line, fmt, args_copy);
    }

    printf("%s %s%-7s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", time_buf, log_level_colors[level], log_level_str[level], file,
           line);
    vprintf(fmt, args);
    printf("\n");
    fflush(stdout);

#if defined(_WIN32)
    va_list args_copy;
    va_copy(args_copy, args);
    uint32_t suffix_size = vsnprintf(nullptr, 0, fmt, args);
    if (suffix_size + MAX_PATH > log_buffer_size)
    {
        log_buffer_size = suffix_size * 2 + MAX_PATH;
        log_buffer = (char*)realloc(log_buffer, log_buffer_size);
    }
    uint32_t prefix_size = snprintf(log_buffer, log_buffer_size, "%s(%u): ", file, line);
    if (prefix_size + suffix_size > log_buffer_size)
    {
        __debugbreak();
    }
    suffix_size = vsnprintf(log_buffer + prefix_size, log_buffer_size - prefix_size, fmt, args_copy);
    OutputDebugStringA(log_buffer);
    OutputDebugStringA("\n");
    va_end(args_copy);
#endif

    va_end(args);
}

void add_log_function(log_function_t function)
{
    ASSERT(num_log_functions < max_log_functions);
    log_functions[num_log_functions++] = function;
}

void remove_log_function(log_function_t function)
{
    uint32_t index = UINT32_MAX;
    for (uint32_t i = 0; i < num_log_functions; ++i)
    {
        if (log_functions[i] == function)
        {
            index = i;
            break;
        }
    }
    if (index != UINT32_MAX)
    {
        log_functions[index] = log_functions[--num_log_functions];
    }
}
