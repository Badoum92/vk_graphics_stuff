#include "core/thread_pool.h"
#include "core/core.h"
#include "core/thread.h"
#include "core/semaphore.h"

#include "moodycamel/concurrentqueue.h"

struct job
{
    thread_pool_job f;
    void* arg;
};

static semaphore semaphore;
static thread* threads;
static uint32_t num_threads;
static uint32_t num_running_threads = 0;
static moodycamel::ConcurrentQueue<job> jobs;
static bool quit = false;

static void thread_pool_function(void*)
{
    job j;
    while (!quit)
    {
        semaphore_wait(&semaphore);
        InterlockedIncrement(&num_running_threads);
        while (jobs.try_dequeue(j))
        {
            j.f(j.arg);
        }
        InterlockedDecrement(&num_running_threads);
    }
}

void thread_pool_init()
{
    num_threads = thread_num_logical_proc() - 1;
    semaphore = semaphore_create(0, num_threads);
    threads = (thread*)malloc(num_threads * sizeof(*threads));

    char buf[32];
    for (uint32_t i = 0; i < num_threads; ++i)
    {
        snprintf(buf, sizeof(buf), "Worker thread #%u", i + 1);
        threads[i] = thread_create(buf, &thread_pool_function, nullptr);
    }
}

void thread_pool_shutdown()
{
    quit = true;
    semaphore_signal(&semaphore, num_threads);
    for (uint32_t i = 0; i < num_threads; ++i)
    {
        thread_join(&threads[i]);
    }
    semaphore_destroy(&semaphore);
    free(threads);
}

void thread_pool_run_job(thread_pool_job job, void* arg)
{
    jobs.enqueue({job, arg});
    semaphore_signal(&semaphore, 1);
}

void thread_pool_wait_empty()
{
    job j;
    while (jobs.try_dequeue(j))
    {
        j.f(j.arg);
    }

    while (InterlockedAdd((LONG*)&num_running_threads, 0) > 0)
    {
        SwitchToThread();
    }
}