#include "core/file.h"

#include <windows.h>

#include "core/core.h"

file file_open_read(const char* path)
{
    HANDLE handle = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT(handle != INVALID_HANDLE_VALUE);
    return {handle};
}

file file_open_write(const char* path)
{
    HANDLE handle = CreateFileA(path, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    ASSERT(handle != INVALID_HANDLE_VALUE);
    return {handle};
}

file file_open_read_no_buffering(const char* path)
{
    HANDLE handle =
        CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_NO_BUFFERING, NULL);
    ASSERT(handle != INVALID_HANDLE_VALUE);
    return {handle};
}

void file_close(file* file)
{
    CloseHandle(file->handle);
}

bool file_is_valid(file* file)
{
    return file->handle != INVALID_HANDLE_VALUE;
}

int64_t file_get_size(file* file)
{
    int64_t size = 0;
    if (!GetFileSizeEx(file->handle, (LARGE_INTEGER*)&size))
        size = 0;
    return size;
}

uint32_t file_get_sector_size(file* file)
{
    FILE_STORAGE_INFO storage_info = {};
    GetFileInformationByHandleEx(file->handle, FileStorageInfo, &storage_info, sizeof(storage_info));
    return storage_info.LogicalBytesPerSector;
}

uint32_t file_read(file* file, void* data, uint32_t size)
{
    DWORD bytes_read = 0;
    ENSURE(ReadFile(file->handle, data, size, &bytes_read, NULL));
    return bytes_read;
}

uint32_t file_write(file* file, void* data, uint32_t size)
{
    DWORD bytes_written = 0;
    ENSURE(WriteFile(file->handle, data, size, &bytes_written, NULL));
    return bytes_written;
}

bool file_seek(file* file, uint32_t position)
{
    LARGE_INTEGER i;
    i.QuadPart = position;
    return SetFilePointerEx(file->handle, i, NULL, FILE_BEGIN);
}

bool file_seek_to_end(file* file)
{
    LARGE_INTEGER i;
    i.QuadPart = 0;
    return SetFilePointerEx(file->handle, i, NULL, FILE_END);
}

bool file_skip(file* file, uint32_t bytes)
{
    LARGE_INTEGER i;
    i.QuadPart = bytes;
    return SetFilePointerEx(file->handle, i, NULL, FILE_CURRENT);
}

int64_t file_tell(file* file)
{
    LARGE_INTEGER position;
    position.QuadPart = UINT32_MAX;
    LARGE_INTEGER i;
    i.QuadPart = 0;
    SetFilePointerEx(file->handle, i, (LARGE_INTEGER*)&position, FILE_CURRENT);
    return position.QuadPart;
}
