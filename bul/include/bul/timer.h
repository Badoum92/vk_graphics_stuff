#pragma once

#include "bul/bul.h"

namespace bul
{
class Timer
{
public:
    Timer();

    double total_s() const;
    double total_ms() const;
    double total_us() const;
    double total_ns() const;

    double elapsed_s();
    double elapsed_ms();
    double elapsed_us();
    double elapsed_ns();

    static constexpr inline size_t S = 1;
    static constexpr inline size_t MS = 1000;
    static constexpr inline size_t US = 1000000;
    static constexpr inline size_t NS = 1000000000;

private:
#ifdef _WIN32
    int64_t perf_frequency_ = 0;
    int64_t start_time_ = 0;
    int64_t prev_time_ = 0;
#endif
};
} // namespace bul
