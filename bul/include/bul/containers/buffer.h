#pragma once

#include "bul/types.h"
#include "bul/containers/span.h"

namespace bul
{
struct buffer : public non_copyable
{
    buffer() = default;
    explicit buffer(size_t size);
    ~buffer();

    buffer(buffer&& other);
    buffer& operator=(buffer&& other);

    void resize(size_t size);

    template <typename T>
    T as()
    {
        return reinterpret_cast<T>(data);
    }

    template <typename T>
    const T as() const
    {
        return reinterpret_cast<T>(data);
    }

    template <typename T>
    operator span<T>()
    {
        return span<T>{as<T>(), size / sizeof(T)};
    }

    template <typename T>
    operator span<const T>() const
    {
        return span<const T>{as<T>(), size / sizeof(T)};
    }

    uint8_t* data = nullptr;
    size_t size = 0;
};
} // namespace bul
