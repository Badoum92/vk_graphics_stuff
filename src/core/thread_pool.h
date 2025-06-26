#pragma once

typedef void (*thread_pool_job)(void*);

void thread_pool_init();
void thread_pool_shutdown();

void thread_pool_run_job(thread_pool_job job, void* arg);
void thread_pool_wait_empty();