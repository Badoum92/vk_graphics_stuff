#pragma once

#include "bul/bul.h"

namespace bul
{
namespace time
{
double total_s();
double total_ms();
double total_us();
double total_ns();

double delta_s();
double delta_ms();
double delta_us();
double delta_ns();

void update();
} // namespace Time

class Timer
{
public:
    Timer();

    double total_s() const;
    double total_ms() const;
    double total_us() const;
    double total_ns() const;

    double delta_s();
    double delta_ms();
    double delta_us();
    double delta_ns();

    void update();

    static constexpr inline int S = 1;
    static constexpr inline int MS = 1'000;
    static constexpr inline int US = 1'000'000;
    static constexpr inline int NS = 1'000'000'000;

private:
#if defined(_WIN32)
    int64_t start_time_ = 0;
    int64_t prev_time_ = 0;
    double delta_ = 0;
#else
    static_assert(false, "Timer unimplemented for this platform");
#endif
};
} // namespace bul
