#include "core/random.h"
#include "core/thread.h"

#include <time.h>

static random new_random()
{
    uint32_t seed = (uint32_t)time(nullptr) * thread_get_num_threads();
    return {seed, seed};
}

static thread_local random _random = new_random();

static uint32_t rand(uint32_t seed)
{
    uint32_t uState = seed * 747796405u + 2891336453u;
    uint32_t uWord = ((uState >> ((uState >> 28u) + 4u)) ^ uState) * 277803737u;
    return ((uWord >> 22u) ^ uWord) & UINT32_MAX;
}

void random_reset()
{
    random_reset(&_random);
}

void random_reset(random* random)
{
    random->current_seed = random->start_seed;
}

float random_float()
{
    return random_float(&_random);
}

float random_float(random* random)
{
    random->current_seed = rand(random->current_seed);
    return random->current_seed / (float)UINT32_MAX;
}

uint32_t random_uint()
{
    return random_uint(&_random);
}

uint32_t random_uint(random* random)
{
    random->current_seed = rand(random->current_seed);
    return random->current_seed;
}

uint32_t random_uint(uint32_t min, uint32_t max)
{
    return random_uint(&_random, min, max);
}

uint32_t random_uint(random* random, uint32_t min, uint32_t max)
{
    random->current_seed = rand(random->current_seed);
    return random->current_seed % (max - min) + min;
}

int32_t random_int()
{
    return random_int(&_random);
}

int32_t random_int(random* random)
{
    random->current_seed = rand(random->current_seed);
    return (int32_t)random->current_seed;
}

int32_t random_int(int32_t min, int32_t max)
{
    return random_int(&_random, min, max);
}

int32_t random_int(random* random, int32_t min, int32_t max)
{
    random->current_seed = rand(random->current_seed);
    return (int32_t)random->current_seed % (max - min) + min;
}