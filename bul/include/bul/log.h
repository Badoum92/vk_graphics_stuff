#pragma once

#include "bul/containers/enum_array.h"

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

static constexpr enum_array<log_level, const char*> log_level_str = {
    "[DEBUG]",
    "[INFO]",
    "[WARNING]",
    "[ERROR]",
};

void log_debug(const char* fmt, ...);
void log_info(const char* fmt, ...);
void log_warning(const char* fmt, ...);
void log_error(const char* fmt, ...);
void log(log_level level, const char* fmt, ...);

using log_callback = void (*)(log_level, const char*);
void add_log_callback(log_callback callback);
void remove_log_callback(log_callback callback);
} // namespace bul
