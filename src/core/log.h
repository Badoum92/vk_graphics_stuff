#pragma once

#include <stdarg.h>

enum LOG_LEVEL
{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_COUNT
};

using log_function_t = void (*)(LOG_LEVEL level, const char* time, const char* file, int line, const char* fmt,
                                va_list va_args);

void _log(LOG_LEVEL level, const char* file, int line, const char* fmt, ...);

#define LOG_DEBUG(...)   _log(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)    _log(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) _log(LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   _log(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG(LEVEL, ...)  _log(LEVEL, __FILE__, __LINE__, __VA_ARGS__)

void add_log_function(log_function_t function);
void remove_log_function(log_function_t function);
