#pragma once

#include <stdint.h>

struct file
{
#if defined(_WIN32)
    void* handle;
#else
#error file unimplemented for this platform
#endif
};

file file_open_read(const char* path);
file file_open_write(const char* path);
file file_open_read_no_buffering(const char* path);

void file_close(file* file);

bool file_is_valid(file* file);

int64_t file_get_size(file* file);
uint32_t file_get_sector_size(file* file);

uint32_t file_read(file* file, void* data, uint32_t size);
uint32_t file_write(file* file, void* data, uint32_t size);

bool file_seek(file* file, uint32_t position);
bool file_seek_to_end(file* file);
bool file_skip(file* file, uint32_t bytes);
int64_t file_tell(file* file);
