#pragma once

#include "bul/containers/enum_array.h"

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

static constexpr EnumArray<LogLevel, const char*> log_level_str = {
    "[DEBUG]",
    "[INFO]",
    "[WARNING]",
    "[ERROR]",
};

void log_debug(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_warning(const char* fmt, ...);
void log_error(const char* fmt, ...);
void log(LogLevel level, const char* fmt, ...);

using log_callback = void (*)(LogLevel, const char*);
void add_log_callback(log_callback callback);
void remove_log_callback(log_callback callback);
} // namespace bul
