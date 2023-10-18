#pragma once

#include "bul/containers/span.h"

namespace bul::cmd_args
{
struct cmd_arg
{
    const char* name = nullptr;
    const char* value = nullptr;
};

void init(int argc, const char* const* argv);
bul::span<const cmd_arg> get_args();

int64_t get_int(const char* name);
float get_float(const char* name);
const char* get_str(const char* name);

int64_t get_int(size_t index);
float get_float(size_t index);
const char* get_str(size_t index);
}