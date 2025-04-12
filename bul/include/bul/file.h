#pragma once

#include <stdint.h>

namespace bul
{
enum class file_mode
{
    read,
    write,
    read_write
};

struct file
{
    static file open(const char* path, file_mode mode);
    static file open_no_buffering(const char* path, file_mode mode);

    void close();

    uint32_t size() const;
    uint32_t sector_size() const;

    uint32_t read(void* data, uint32_t size);
    uint32_t write(void* data, uint32_t size);

    bool seek(uint32_t position);
    bool seek_to_end();
    bool skip(uint32_t bytes);
    uint64_t tell() const;

#if defined(_WIN32)
    void* handle;
#else
#error file unimplemented for this platform
#endif
};
} // namespace bul
