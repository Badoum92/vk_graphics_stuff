#include "bul/buffer.h"

#include <cstdlib>
#include <utility>

#include "bul/bul.h"

namespace bul
{
buffer::buffer(size_t size_)
{
    data = static_cast<T*>(malloc(size_));
    ASSERT(data != nullptr);
    size = size_;
}

buffer::~buffer()
{
    free(data);
    data = nullptr;
    size_ = 0;
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

    data = static_cast<T*>(realloc(data, size_));
    ASSERT(data != nullptr);
    size = size_;
}
} // namespace bul