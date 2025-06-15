#include "core/time.h"

#include <windows.h>

static int64_t frame_start = 0;
static int64_t frame_delta = 0;
static float frame_start_s = 0;
static float frame_delta_s = 0;

static int64_t get_perf_frequency()
{
    int64_t frequency = 0;
    QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    return frequency;
}

static int64_t get_perf_counter()
{
    int64_t counter = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&counter);
    return counter;
}

static const int64_t perf_frequency = get_perf_frequency();
static const int64_t start_perf_counter = get_perf_counter();

int64_t time_now()
{
    return get_perf_counter() - start_perf_counter;
}

float time_to_s(int64_t time)
{
    return (time * 1.0f) / perf_frequency;
}

float time_to_ms(int64_t time)
{
    return (time * 1'000.0f) / perf_frequency;
}

float time_to_us(int64_t time)
{
    return (time * 1'000'000.0f) / perf_frequency;
}

float time_to_ns(int64_t time)
{
    return (time * 1'000'000'000.0f) / perf_frequency;
}

int64_t time_update()
{
    int64_t now = time_now();
    frame_delta = now - frame_start;
    frame_start = now;
    frame_start_s = time_to_s(frame_start);
    frame_delta_s = time_to_s(frame_delta);
    return now;
}

int64_t time_get_delta()
{
    return frame_delta;
}

float time_get_delta_s()
{
    return frame_delta_s;
}

int64_t time_get_last_update()
{
    return frame_start;
}

float time_get_last_update_s()
{
    return frame_start_s;
}
