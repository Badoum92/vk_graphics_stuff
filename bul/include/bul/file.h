#pragma once

#include <cstdint>

namespace bul
{
size_t file_size(const char* path);
bool read_file(const char* path, uint8_t* data, size_t size);
bool write_file(const char* path, const uint8_t* data, size_t size);
} // namespace bul
