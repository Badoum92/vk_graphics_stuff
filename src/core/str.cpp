#include "core/str.h"

#include <string.h>

bool str_ends_with(const char* str, const char* end)
{
    size_t str_len = strlen(str);
    size_t end_len = strlen(end);
    if (end_len > str_len)
        return false;
    return strncmp(str + str_len - end_len, end, end_len);
}