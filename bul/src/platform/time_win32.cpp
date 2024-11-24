#include "bul/time.h"

#include <windows.h>

namespace bul
{
int64_t frame_start_tick = 0;
int64_t frame_delta_ticks = 0;
int64_t avg_frame_delta_ticks = 0;
float frame_start_s = 0;
float frame_delta_s = 0;

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

int64_t current_tick()
{
    return get_perf_counter() - start_perf_counter;
}

int64_t ticks_to_s(int64_t ticks)
{
    return (ticks * 1) / perf_frequency;
}

int64_t ticks_to_ms(int64_t ticks)
{
    return (ticks * 1'000) / perf_frequency;
}

int64_t ticks_to_us(int64_t ticks)
{
    return (ticks * 1'000'000) / perf_frequency;
}

float ticks_to_s_f(int64_t ticks)
{
    return (ticks * 1.0f) / perf_frequency;
}

float ticks_to_ms_f(int64_t ticks)
{
    return (ticks * 1'000.0f) / perf_frequency;
}

float ticks_to_us_f(int64_t ticks)
{
    return (ticks * 1'000'000.0f) / perf_frequency;
}

void time_update()
{
    int64_t current_tick = bul::current_tick();
    frame_delta_ticks = current_tick - frame_start_tick;
    frame_start_tick = current_tick;
    frame_start_s = bul::ticks_to_s_f(frame_start_tick);
    frame_delta_s = bul::ticks_to_s_f(frame_delta_ticks);
    avg_frame_delta_ticks = avg_frame_delta_ticks * 0.9f + frame_delta_ticks * 0.1f;
}
} // namespace bul
