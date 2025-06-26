#pragma once

#include <stdint.h>

struct random
{
    uint32_t start_seed;
    uint32_t current_seed;
};

void random_reset();
void random_reset(random* random);

float random_float();
float random_float(random* random);

uint32_t random_uint();
uint32_t random_uint(random* random);

uint32_t random_uint(uint32_t min, uint32_t max);
uint32_t random_uint(random* random, uint32_t min, uint32_t max);

int32_t random_int();
int32_t random_int(random* random);

int32_t random_int(int32_t min, int32_t max);
int32_t random_int(random* random, int32_t min, int32_t max);