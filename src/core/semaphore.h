#pragma once

#include <stdint.h>

struct semaphore
{
#if defined(_WIN32)
    void* handle;
#endif
};

semaphore semaphore_create(int32_t count = 0, uint32_t max_count = INT32_MAX);
void semaphore_destroy(semaphore* semaphore);
bool semaphore_wait(semaphore* semaphore);
bool semaphore_try_wait(semaphore* semaphore);
void semaphore_signal(semaphore* semaphore, int32_t count = 1);