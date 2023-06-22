#include "bul/format.h"

#include <cstdio>

#include "bul/containers/buffer.h"

namespace bul
{
static thread_local Buffer<char> log_buffer;

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
    size_t size = vsnprintf(nullptr, 0, fmt, args);
    if (size >= log_buffer.size())
    {
        log_buffer.resize(size + 1);
    }
    vsnprintf(log_buffer.data(), log_buffer.size(), fmt, args_copy);
    return log_buffer.data();
}
} // namespace bul
