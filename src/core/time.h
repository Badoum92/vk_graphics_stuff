#pragma once

#include <stdint.h>

int64_t time_now();

float time_to_s(int64_t time);
float time_to_ms(int64_t time);
float time_to_us(int64_t time);
float time_to_ns(int64_t time);

int64_t time_update();
int64_t time_get_delta();
float time_get_delta_s();
int64_t time_get_last_update();
float time_get_last_update_s();
