#pragma once

#include <string_view>

#include "bul/bul.h"

namespace bul
{
struct UUID
{
    uint32_t a;
    uint16_t b;
    uint16_t c;
    uint16_t d;
    uint8_t e[6];

    UUID(std::string_view s)
    {
        ASSERT(s.size() == 36);

        constexpr auto ascii_to_number = [](char c) -> uint8_t { return (c >= 'a') ? c - 'a' + 10 : c - '0'; };
        auto data = reinterpret_cast<uint8_t*>(this);
        size_t data_i = 0;
        size_t total_i = 0;

        for (char c : s)
        {
            if (c == '-')
            {
                continue;
            }

            uint8_t n = ascii_to_number(c);
            if (total_i % 2 == 0)
            {
                data[data_i] = (n << 4);
            }
            else
            {
                data[data_i++] |= n;
            }
            ++total_i;
        }
    }

    bool operator==(const UUID& other) const;

    void to_string(char (&s)[37])
    {
        constexpr auto number_to_ascii = [](uint8_t n) -> uint8_t { return (n >= 10) ? n + 'a' - 10 : n + '0' };
        auto lambda = [](char(&s)[37], size_t& data_i, size_t& s_i, size_t n) {
            for (size_t i = 0; i < n; ++i)
            {
                s[s_i]
            }
        }
    }
};

static_assert(sizeof(UUID) == 16);
} // namespace bul
