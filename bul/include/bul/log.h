#pragma once

namespace bul
{
enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error,
    Count,
};

void log_debug(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_warning(const char* fmt, ...);
void log_error(const char* fmt, ...);
void log(LogLevel level, const char* fmt, ...);
} // namespace bul
