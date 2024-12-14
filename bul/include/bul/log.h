#pragma once

#include <stdarg.h>

namespace bul
{
enum log_level
{
    log_level_debug,
    log_level_info,
    log_level_warning,
    log_level_error,
    _log_level_count,
};

using log_function_t = void (*)(log_level level, const char* time, const char* file, int line, const char* fmt,
                                va_list va_args);

void _log(log_level level, const char* file, int line, const char* fmt, ...);

#define log_debug(...)   _log(bul::log_level_debug, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...)    _log(bul::log_level_info, __FILE__, __LINE__, __VA_ARGS__)
#define log_warning(...) _log(bul::log_level_warning, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...)   _log(bul::log_level_error, __FILE__, __LINE__, __VA_ARGS__)
#define log(LEVEL, ...)  _log(LEVEL, __FILE__, __LINE__, __VA_ARGS__)

void add_log_function(log_function_t function);
void remove_log_function(log_function_t function);
} // namespace bul
