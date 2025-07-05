#include "core/thread.h"

#include <windows.h>
#include <stdlib.h>

#include "core/core.h"
#include "core/math/math.h"

static uint32_t num_threads = uint32_t(-1);
static thread_local uint32_t id = uint32_t(-1);

struct thread_param
{
    thread_function function;
    void* arg;
};

static DWORD thread_run(LPVOID arg)
{
    id = InterlockedIncrement(&num_threads);
    thread_param* param = (thread_param*)arg;
    param->function(param->arg);
    free(param);
    return 0;
}

thread thread_create(const char* name, thread_function function, void* arg)
{
    thread_param* param = (thread_param*)malloc(sizeof(*param));
    param->function = function;
    param->arg = arg;

    HANDLE thread_handle = CreateThread(nullptr, 0, thread_run, param, 0, nullptr);
    ASSERT(thread_handle != INVALID_HANDLE_VALUE);

    wchar_t wname[64] = {0};
    mbstowcs(wname, name, MIN(ARRAY_SIZE(wname), strlen(name)));
    SetThreadDescription(thread_handle, wname);

    return {thread_handle};
}

void thread_join(thread* thread)
{
    WaitForSingleObject(thread->handle, INFINITE);
}

void thread_sleep(uint32_t ms)
{
    Sleep(ms);
}

uint32_t thread_get_num_threads()
{
    return num_threads;
}

uint32_t thread_get_id()
{
    return id;
}

void thread_init()
{
    id = 0;
    num_threads = 0;
}

static uint32_t num_logical_proc = []() {
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = nullptr;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = nullptr;
    DWORD length = 0;
    GetLogicalProcessorInformation(buffer, &length);
    buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(length);
    GetLogicalProcessorInformation(buffer, &length);
    ptr = buffer;
    DWORD count = 0;
    DWORD offset = 0;
    while (offset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= length)
    {
        if (ptr->Relationship == RelationProcessorCore)
        {
            count += (uint32_t)__popcnt64(ptr->ProcessorMask);
        }
        offset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
        ptr++;
    }
    free(buffer);
    return count;
}();

uint32_t thread_num_logical_proc()
{
    return num_logical_proc;
}