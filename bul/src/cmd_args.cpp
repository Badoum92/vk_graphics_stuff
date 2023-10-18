#include "bul/cmd_args.h"

#include <cstdio>

#include "bul/containers/vector.h"

namespace bul::cmd_args
{
static bul::vector<cmd_arg> cmd_args;

void init(int argc, const char* const* argv)
{
    for (int i = 0; i < argc; ++i)
    {
        const char* current = argv[i];
        const char* next = argv[i + 1];
        cmd_arg& cmd_arg = cmd_args.emplace_back();

        size_t name_offset = current[0] == '-' + (current[0] == '-' && current[1] == '-');
        cmd_arg.name = current + name_offset;

        if (next && next[0] != '-')
        {
            cmd_arg.value = next;
        }
    }
}

bul::span<const cmd_arg> get_args()
{
    return cmd_args;
}

const vector<cmd_arg>& get_cmd_args()
{
    return cmd_args;
}

int64_t get_int(const char* name)
{
    const char* value = get_str(name);
    int64_t i = 0;
    if (value)
    {
        sscanf_s(value, "%lld", &i);
    }
    return i;
}

float get_float(const char* name)
{
    const char* value = get_str(name);
    float f = 0;
    if (value)
    {
        sscanf_s(value, "%f", &f);
    }
    return f;
}

const char* get_str(const char* name)
{
    for (const auto& arg : cmd_args)
    {
        if (!strcmp(name, arg.name))
        {
            return arg.value;
        }
    }
    return nullptr;
}

int64_t get_int(size_t index)
{
    const char* value = get_str(index);
    int64_t i = 0;
    if (value)
    {
        sscanf_s(value, "%lld", &i);
    }
    return i;
}

float get_float(size_t index)
{
    const char* value = get_str(index);
    float f = 0;
    if (value)
    {
        sscanf_s(value, "%f", &f);
    }
    return f;
}

const char* get_str(size_t index)
{
    ASSERT(index < cmd_args.size());
    return cmd_args[index].value;
}
} // namespace bul::cmd_args
