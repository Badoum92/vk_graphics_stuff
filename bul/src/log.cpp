#include "bul/log.h"

#include <cstdio>
#include <cstdarg>

#include "bul/format.h"
#include "bul/containers/enum_array.h"

namespace bul
{
static constexpr EnumArray<LogLevel, const char*> log_level_str = {
    "[DEBUG]",
    "[INFO]",
    "[WARNING]",
    "[ERROR]",
};

static void log(LogLevel level, const char* fmt, va_list args)
{
    const char* text = format(fmt, args);
    printf("%s %s\n", log_level_str[level], text);
}

void log_debug(const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);
    log(LogLevel::Debug, fmt, args);
    va_end(args);
}

void log_info(const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);
    log(LogLevel::Info, fmt, args);
    va_end(args);
}

void log_warning(const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);
    log(LogLevel::Warning, fmt, args);
    va_end(args);
}

void log_error(const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);
    log(LogLevel::Error, fmt, args);
    va_end(args);
}

void log(LogLevel level, const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);
    log(level, fmt, args);
    va_end(args);
}
} // namespace bul
