#include "bul/hash.h"

#include "xxhash/xxhash.h"

namespace bul
{
uint64_t hash(const void* data, size_t size)
{
    return XXH64(data, size, 0);
}
}; // namespace bul
