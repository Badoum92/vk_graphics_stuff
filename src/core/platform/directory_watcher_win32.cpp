#include "core/directory_watcher.h"
#include "core/core.h"

#include <windows.h>

static void directory_watcher_start(directory_watcher* watcher);

static FILE_NOTIFY from_win32(DWORD file_action)
{
    switch (file_action)
    {
    case FILE_ACTION_ADDED:
        return FILE_NOTIFY_ADDED;
    case FILE_ACTION_REMOVED:
        return FILE_NOTIFY_REMOVED;
    case FILE_ACTION_MODIFIED:
        return FILE_NOTIFY_MODIFIED;
    case FILE_ACTION_RENAMED_OLD_NAME:
        return FILE_NOTIFY_RENAMED_OLD_NAME;
    case FILE_ACTION_RENAMED_NEW_NAME:
        return FILE_NOTIFY_RENAMED_NEW_NAME;
    }
    return FILE_NOTIFY_COUNT;
}

static void directory_watcher_process_change(directory_watcher* watcher)
{
    char file_name[MAX_PATH] = {};
    char* base = watcher->back_buffer;
    for (;;)
    {
        FILE_NOTIFY_EXTENDED_INFORMATION* notify_info = (FILE_NOTIFY_EXTENDED_INFORMATION*)base;
        size_t written = wcstombs(file_name, notify_info->FileName, notify_info->FileNameLength / 2);
        file_name[written] = 0;
        if (watcher->callback)
            watcher->callback(file_name, from_win32(notify_info->Action));
        if (notify_info->NextEntryOffset == 0)
            break;
        base += notify_info->NextEntryOffset;
    }
}

static void directory_watcher_completion(DWORD error, DWORD bytes_read, OVERLAPPED* overlapped)
{
    directory_watcher* watcher = (directory_watcher*)overlapped->hEvent;

    if (error == ERROR_OPERATION_ABORTED)
    {
        return;
    }

    ASSERT(bytes_read >= offsetof(FILE_NOTIFY_EXTENDED_INFORMATION, FileName) + sizeof(WCHAR));
    if (bytes_read == 0)
    {
        return;
    }

    memcpy(watcher->back_buffer, watcher->buffer, KB(16));
    memset(watcher->buffer, 0, KB(16));

    directory_watcher_start(watcher);
    directory_watcher_process_change(watcher);
}

static void directory_watcher_start(directory_watcher* watcher)
{
    DWORD bytes = 0;
    ENSURE(ReadDirectoryChangesExW(
        watcher->handle, watcher->buffer, KB(16), FALSE,
        FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME, &bytes,
        (OVERLAPPED*)watcher->overlapped, &directory_watcher_completion, ReadDirectoryNotifyExtendedInformation));
}

static void directory_watcher_thread(void* arg)
{
    directory_watcher* watcher = (directory_watcher*)arg;
    watcher->buffer = (char*)malloc(KB(16));
    watcher->back_buffer = (char*)malloc(KB(16));
    memset(watcher->buffer, 0, KB(16));
    memset(watcher->back_buffer, 0, KB(16));

    directory_watcher_start(watcher);
    while (watcher->thread_run)
    {
        ::SleepEx(1000, TRUE);
    }

    free(watcher->buffer);
    free(watcher->back_buffer);
}

void directory_watcher_run(directory_watcher* watcher, const char* directory, directory_watcher_callback callback)
{
    strncpy(watcher->directory, directory, sizeof(watcher->directory));

    wchar_t wdirectory[MAX_PATH * 2];
    mbstowcs(wdirectory, directory, MAX_PATH);

    watcher->handle =
        CreateFileW(wdirectory, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
                    OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, nullptr);
    ASSERT(GetLastError() != ERROR_FILE_NOT_FOUND);

    memset(watcher->overlapped, 0, sizeof(watcher->overlapped));
    ((OVERLAPPED*)watcher->overlapped)->hEvent = watcher;

    watcher->callback = callback;
    watcher->thread_run = true;
    watcher->thread = thread_create("watcher", &directory_watcher_thread, watcher);
}

void directory_watcher_stop(directory_watcher* watcher)
{
    watcher->thread_run = false;
    thread_join(&watcher->thread);
}
