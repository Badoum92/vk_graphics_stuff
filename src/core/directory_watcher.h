#pragma once

#include <stdint.h>

#include "core/thread.h"

enum FILE_NOTIFY
{
    FILE_NOTIFY_ADDED,
    FILE_NOTIFY_REMOVED,
    FILE_NOTIFY_MODIFIED,
    FILE_NOTIFY_RENAMED_OLD_NAME,
    FILE_NOTIFY_RENAMED_NEW_NAME,
    FILE_NOTIFY_COUNT,
};

typedef void (*directory_watcher_callback)(const char* file, FILE_NOTIFY notify);

struct directory_watcher
{
#if defined(_WIN32)
    char* back_buffer;
    char* buffer;
    uint8_t overlapped[32];
    void* handle;
    char directory[512];
    bool thread_run;
    thread thread;
    directory_watcher_callback callback;
#endif
};

void directory_watcher_run(directory_watcher* watcher, const char* directory, directory_watcher_callback callback);
void directory_watcher_stop(directory_watcher* watcher);