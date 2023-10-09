#include "bul/format.h"

#include <cstdio>

#include "bul/containers/buffer.h"

namespace bul
{
static thread_local buffer log_buffer;

const char* format(const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);
    const char* text = format(fmt, args);
    va_end(args);
    return text;
}

const char* format(const char* fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    int size = vsnprintf(nullptr, 0, fmt, args);
    ASSERT(size > 0);
    if (size_t(size) >= log_buffer.size)
    {
        log_buffer.resize(size_t(size) + 1);
    }
    vsnprintf(log_buffer.as<char*>(), log_buffer.size, fmt, args_copy);
    return log_buffer.as<char*>();
}
} // namespace bul
