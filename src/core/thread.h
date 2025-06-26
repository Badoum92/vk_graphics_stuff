#pragma once

#include <stdint.h>

struct thread
{
#if defined(_WIN32)
    void* handle;
#endif
};

typedef void (*thread_function)(void* arg);

thread thread_create(const char* name, thread_function function, void* arg);
void thread_join(thread* thread);
void thread_sleep(uint32_t ms);

uint32_t thread_get_num_threads();
uint32_t thread_get_id();
void thread_init();

uint32_t thread_num_logical_proc();