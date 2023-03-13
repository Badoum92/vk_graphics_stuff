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
static Timer global_timer;

namespace time
{
double total_s()
{
    return global_timer.total_s();
}

double total_ms()
{
    return global_timer.total_ms();
}

double total_us()
{
    return global_timer.total_us();
}

double total_ns()
{
    return global_timer.total_ns();
}

double delta_s()
{
    return global_timer.delta_s();
}

double delta_ms()
{
    return global_timer.delta_ms();
}

double delta_us()
{
    return global_timer.delta_us();
}

double delta_ns()
{
    return global_timer.delta_ns();
}

void update()
{
    global_timer.update();
}
} // namespace Time

Timer::Timer()
{
    start_time_ = get_perf_counter();
    prev_time_ = start_time_;
}

double Timer::total_s() const
{
    return double(get_perf_counter() - start_time_) * S / perf_frequency;
}

double Timer::total_ms() const
{
    return double(get_perf_counter() - start_time_) * MS / perf_frequency;
}

double Timer::total_us() const
{
    return double(get_perf_counter() - start_time_) * US / perf_frequency;
}

double Timer::total_ns() const
{
    return double(get_perf_counter() - start_time_) * NS / perf_frequency;
}

double Timer::delta_s()
{
    return delta_ * S;
}

double Timer::delta_ms()
{
    return delta_ * MS;
}

double Timer::delta_us()
{
    return delta_ * US;
}

double Timer::delta_ns()
{
    return delta_ * NS;
}

void Timer::update()
{
    int64_t cur_time = get_perf_counter();
    delta_ = double(cur_time - prev_time_) / perf_frequency;
    prev_time_ = cur_time;
}
} // namespace bul
