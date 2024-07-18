#pragma once

namespace bul
{
enum class log_level
{
    debug,
    info,
    warning,
    error,
    _count,
};

#define log_debug(...) _log(bul::log_level::debug, __FILE__, __LINE__, __VA_ARGS__)
#define log_info(...) _log(bul::log_level::info, __FILE__, __LINE__, __VA_ARGS__)
#define log_warning(...) _log(bul::log_level::warning, __FILE__, __LINE__, __VA_ARGS__)
#define log_error(...) _log(bul::log_level::error, __FILE__, __LINE__, __VA_ARGS__)
#define log(LEVEL, ...) _log(LEVEL, __FILE__, __LINE__, __VA_ARGS__)

void _log(log_level level, const char* file, int line, const char* fmt, ...);
} // namespace bul
