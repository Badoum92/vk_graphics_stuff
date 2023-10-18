#include "bul/time.h"

#include <windows.h>

namespace bul
{
static int64_t get_perf_frequency()
{
    uint64_t perf_freq = 0;
    QueryPerformanceFrequency((LARGE_INTEGER*)&perf_freq);
    return perf_freq;
}

static int64_t get_perf_counter()
{
    int64_t perf_counter = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&perf_counter);
    return perf_counter;
}

static const int64_t perf_frequency = get_perf_frequency();
static timer global_timer;
static timer delta_timer;
static float delta_time_s;
static float delta_time_ms;
static float delta_time_us;
static float delta_time_ns;

namespace time
{
float total_s()
{
    return global_timer.get_s();
}

float total_ms()
{
    return global_timer.get_ms();
}

float total_us()
{
    return global_timer.get_us();
}

float total_ns()
{
    return global_timer.get_ns();
}

float delta_s()
{
    float time = delta_timer.get_s();
    delta_timer.reset();
    return time;
}

float delta_ms()
{
    float time = delta_timer.get_s();
    delta_timer.reset();
    return time;
}

float delta_us()
{
    float time = delta_timer.get_s();
    delta_timer.reset();
    return time;
}

float delta_ns()
{
    float time = delta_timer.get_s();
    delta_timer.reset();
    return time;
}

void update()
{
    delta_time_ns = delta_timer.get_ns();
    delta_timer.reset();
    delta_time_us = delta_time_ns / 1'000.0f;
    delta_time_ms = delta_time_ns / 1'000'000.0f;
    delta_time_s = delta_time_ns / 1'000'000'000.0f;
}
} // namespace time

timer::timer()
{
    reset();
}

void timer::reset()
{
    _start_time = get_perf_counter();
}

float timer::get_s() const
{
    return float(get_perf_counter() - _start_time) * time::S / perf_frequency;
}

float timer::get_ms() const
{
    return float(get_perf_counter() - _start_time) * time::MS / perf_frequency;
}

float timer::get_us() const
{
    return float(get_perf_counter() - _start_time) * time::US / perf_frequency;
}

float timer::get_ns() const
{
    return float(get_perf_counter() - _start_time) * time::NS / perf_frequency;
}

} // namespace bul
