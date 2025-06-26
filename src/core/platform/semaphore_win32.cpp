#include "core/semaphore.h"
#include "core/core.h"

semaphore semaphore_create(int32_t count, uint32_t max_count)
{
    HANDLE handle = CreateSemaphoreW(nullptr, count, max_count, nullptr);
    ENSURE(handle);
    return {handle};
}

void semaphore_destroy(semaphore* semaphore)
{
    CloseHandle(semaphore->handle);
}

bool semaphore_wait(semaphore* semaphore)
{
    return WaitForSingleObject(semaphore->handle, INFINITE) == 0;
}

bool semaphore_try_wait(semaphore* semaphore)
{
    return WaitForSingleObject(semaphore->handle, 0) == 0;
}

void semaphore_signal(semaphore* semaphore, int32_t count)
{
    ReleaseSemaphore(semaphore->handle, count, nullptr);
}