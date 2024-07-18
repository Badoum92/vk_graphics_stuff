#include "bul/file.h"

#include <windows.h>

#include "bul/bul.h"

namespace bul
{
static DWORD get_win32_access_mode(file::mode mode)
{
    switch (mode)
    {
    case file::mode::read:
        return GENERIC_READ;
    case file::mode::write:
        return GENERIC_WRITE;
    case file::mode::read_write:
        return GENERIC_READ | GENERIC_WRITE;
    }
    return 0;
}

static DWORD get_win32_create_mode(file::mode mode)
{
    switch (mode)
    {
    case file::mode::read:
        return OPEN_EXISTING;
    case file::mode::write:
        return OPEN_ALWAYS;
    case file::mode::read_write:
        return OPEN_ALWAYS;
    }
    return 0;
}

file file::open(const char* path, mode mode)
{
    HANDLE handle = CreateFileA(path, get_win32_access_mode(mode), 0, NULL, get_win32_create_mode(mode),
                                FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT(handle != INVALID_HANDLE_VALUE);
    return file{handle};
}

file file::open_no_buffering(const char* path, mode mode)
{
    HANDLE handle = CreateFileA(path, get_win32_access_mode(mode), 0, NULL, get_win32_create_mode(mode),
                                FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);
    ASSERT(handle != INVALID_HANDLE_VALUE);
    return file{handle};
}

void file::close()
{
    CloseHandle(handle);
}

uint32_t file::size() const
{
    int64_t size = 0;
    GetFileSizeEx(handle, (LARGE_INTEGER*)&size);
    return (uint32_t)size;
}

uint32_t file::sector_size() const
{
    FILE_STORAGE_INFO storage_info = {};
    GetFileInformationByHandleEx(handle, FileStorageInfo, &storage_info, sizeof(storage_info));
    return storage_info.LogicalBytesPerSector;
}

uint32_t file::read(void* data, uint32_t size)
{
    DWORD bytes_read = 0;
    ENSURE(ReadFile(handle, data, size, &bytes_read, NULL));
    return bytes_read;
}

uint32_t file::write(void* data, uint32_t size)
{
    DWORD bytes_written = 0;
    ENSURE(WriteFile(handle, data, size, &bytes_written, NULL));
    return bytes_written;
}

bool file::seek(uint32_t position)
{
    LARGE_INTEGER i;
    i.QuadPart = position;
    return SetFilePointerEx(handle, i, NULL, FILE_BEGIN);
}

bool file::seek_to_end()
{
    LARGE_INTEGER i;
    i.QuadPart = 0;
    return SetFilePointerEx(handle, i, NULL, FILE_END);
}

bool file::skip(uint32_t bytes)
{
    LARGE_INTEGER i;
    i.QuadPart = bytes;
    return SetFilePointerEx(handle, i, NULL, FILE_CURRENT);
}

uint64_t file::tell() const
{
    LARGE_INTEGER position;
    position.QuadPart = UINT32_MAX;
    LARGE_INTEGER i;
    i.QuadPart = 0;
    SetFilePointerEx(handle, i, (LARGE_INTEGER*)&position, FILE_CURRENT);
    return (uint64_t)position.QuadPart;
}
} // namespace bul
