#include "core/thread.h"

#include <windows.h>
#include <stdlib.h>

#include "core/core.h"
#include "core/math/math.h"

static uint32_t num_thread = uint32_t(-1);
static thread_local uint32_t id = uint32_t(-1);

struct thread_param
{
    thread_function function;
    void* arg;
};

static DWORD thread_run(LPVOID arg)
{
    id = InterlockedIncrement(&num_thread);
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
    mbstowcs(wname, name, math_min(ARRAY_SIZE(wname), strlen(name)));
    SetThreadDescription(thread_handle, wname);

    return {thread_handle};
}

void thread_join(thread* thread)
{
    WaitForSingleObject(thread->handle, INFINITE);
}

uint32_t thread_get_id()
{
    return id;
}

void thread_init()
{
    id = 0;
    num_thread = 0;
}
