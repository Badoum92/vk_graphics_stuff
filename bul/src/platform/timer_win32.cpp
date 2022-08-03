#include "bul/timer.h"

#include <windows.h>

namespace bul
{
static int64_t get_perf_counter()
{
    int64_t perf_counter = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&perf_counter);
    return perf_counter;
}

Timer::Timer()
{
    QueryPerformanceFrequency((LARGE_INTEGER*)&perf_frequency_);
    start_time_ = get_perf_counter();
    prev_time_ = start_time_;
}

template<size_t N>
static double total(int64_t start_time, int64_t perf_frequency)
{
    return double(get_perf_counter() - start_time) * N / perf_frequency;
}

double Timer::total_s() const
{
    return total<S>(start_time_, perf_frequency_);
}

double Timer::total_ms() const
{
    return total<MS>(start_time_, perf_frequency_);
}

double Timer::total_us() const
{
    return total<US>(start_time_, perf_frequency_);
}

double Timer::total_ns() const
{
    return total<NS>(start_time_, perf_frequency_);
}

template <size_t N>
static double elapsed(int64_t& prev_time, int64_t perf_frequency)
{
    int64_t cur_time = get_perf_counter();
    double elapsed = double(cur_time - prev_time) * N / perf_frequency;
    prev_time = cur_time;
    return elapsed;
}

double Timer::elapsed_s()
{
    return elapsed<S>(prev_time_, perf_frequency_);
}

double Timer::elapsed_ms()
{
    return elapsed<MS>(prev_time_, perf_frequency_);
}

double Timer::elapsed_us()
{
    return elapsed<US>(prev_time_, perf_frequency_);
}

double Timer::elapsed_ns()
{
    return elapsed<NS>(prev_time_, perf_frequency_);
}
} // namespace bul
