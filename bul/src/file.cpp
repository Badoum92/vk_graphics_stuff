#include "bul/file.h"

#include <cstdio>

namespace bul
{
static size_t file_size(FILE* file)
{
    if (fseek(file, 0, SEEK_END) != 0)
    {
        return 0;
    }
    int64_t size = _ftelli64(file);
    return size != -1 ? size : 0;
}

size_t file_size(const char* path)
{
    FILE* file = nullptr;
    if (fopen_s(&file, path, "rb") != 0)
    {
        return 0;
    }
    size_t size = file_size(file);
    fclose(file);
    return size;
}

bool read_file(const char* path, uint8_t* data, size_t size)
{
    FILE* file = nullptr;
    if (fopen_s(&file, path, "rb") != 0)
    {
        return false;
    }
    size_t read = fread(data, 1, size, file);
    fclose(file);
    return read == size;
}

bool read_file(const char* path, std::vector<char>& data)
{
    FILE* file = nullptr;
    if (fopen_s(&file, path, "rb") != 0)
    {
        return false;
    }
    size_t size = file_size(file);
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        fclose(file);
        return 0;
    }
    data.resize(size);
    size_t read = fread(data.data(), 1, size, file);
    fclose(file);
    return read == size;
}

bool write_file(const char* path, const uint8_t* data, size_t size)
{
    FILE* file = nullptr;
    if (fopen_s(&file, path, "wb") != 0)
    {
        return false;
    }
    size_t written = fwrite(data, 1, size, file);
    fclose(file);
    return written = size;
}
} // namespace bul
