#pragma once

#include "bul/bul.h"

namespace bul
{
namespace time
{
inline constexpr int S = 1;
inline constexpr int MS = 1'000;
inline constexpr int US = 1'000'000;
inline constexpr int NS = 1'000'000'000;

float total_s();
float total_ms();
float total_us();
float total_ns();

float delta_s();
float delta_ms();
float delta_us();
float delta_ns();

void update();
} // namespace Time

struct timer
{
    timer();

    void reset();

    float get_s() const;
    float get_ms() const;
    float get_us() const;
    float get_ns() const;

#if defined(_WIN32)
    int64_t _start_time;
#else
    static_assert(false, "Timer unimplemented for this platform");
#endif
};
} // namespace bul
