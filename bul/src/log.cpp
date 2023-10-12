#include "bul/log.h"

#include <cstdio>
#include <cstdarg>

#include "bul/format.h"
#include "bul/containers/vector.h"

namespace bul
{
static vector<log_callback> callbacks;

static void log(LogLevel level, const char* fmt, va_list args)
{
    const char* text = format(fmt, args);
    printf("%s %s\n", log_level_str[level], text);
    for (log_callback callback : callbacks)
    {
        callback(level, text);
    }
}

void log_debug(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(LogLevel::debug, fmt, args);
    va_end(args);
}

void log_info(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(LogLevel::info, fmt, args);
    va_end(args);
}

void log_warning(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(LogLevel::warning, fmt, args);
    va_end(args);
}

void log_error(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(LogLevel::error, fmt, args);
    va_end(args);
}

void log(LogLevel level, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log(level, fmt, args);
    va_end(args);
}

void add_log_callback(log_callback callback)
{
    callbacks.emplace_back(callback);
}

void remove_log_callback(log_callback callback)
{
    callbacks.erase(callbacks.find(callback));
}
} // namespace bul
