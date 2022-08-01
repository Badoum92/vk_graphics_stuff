#include "bul/bul.h"

#include <cstdio>

namespace bul
{
void internal_assert(bool cond, const char* cond_str, const char* msg, const char* file, unsigned line)
{
    if (!cond)
    {
        fprintf(stderr, "Assertion (%s) failed in %s line %u", cond_str, file, line);
        if (msg != nullptr)
        {
            fprintf(stderr, ": %s\n", msg);
        }
        __debugbreak();
    }
}
} // namespace bul
