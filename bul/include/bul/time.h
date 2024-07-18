#pragma once

#include <stdint.h>

namespace bul
{
int64_t current_tick();

int64_t ticks_to_s(int64_t ticks);
int64_t ticks_to_ms(int64_t ticks);
int64_t ticks_to_us(int64_t ticks);

float ticks_to_s_f(int64_t ticks);
float ticks_to_ms_f(int64_t ticks);
float ticks_to_us_f(int64_t ticks);
} // namespace bul
