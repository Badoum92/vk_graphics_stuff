#include "bul/containers/buffer.h"

#include <cstdlib>
#include <utility>

#include "bul/bul.h"

namespace bul
{
buffer::buffer(size_t size_)
{
    data = reinterpret_cast<uint8_t*>(malloc(size_));
    ASSERT(data != nullptr);
    size = size_;
}

buffer::~buffer()
{
    free(data);
    data = nullptr;
    size = 0;
}

buffer::buffer(buffer&& other)
{
    *this = std::move(other);
}

buffer& buffer::operator=(buffer&& other)
{
    free(data);
    data = other.data;
    size = other.size;
    other.data = nullptr;
    other.size = 0;
    return *this;
}

void buffer::resize(size_t size_)
{
    if (size > size_)
    {
        return;
    }

    data = reinterpret_cast<uint8_t*>(realloc(data, size_));
    ASSERT(data != nullptr);
    size = size_;
}
} // namespace bul