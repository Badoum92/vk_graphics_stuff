#pragma once

#include "bul/bul.h"

#include <string_view>
#include <concepts>

namespace bul
{
namespace _private
{
// hash function adapted from: https://github.com/wangyi-fudan/wyhash/blob/master/wyhash.h
inline void mum(uint64_t* a, uint64_t* b) noexcept
{
#if defined(__SIZEOF_INT128__)
    __uint128_t r = *a;
    r *= *b;
    *a = static_cast<uint64_t>(r);
    *b = static_cast<uint64_t>(r >> 64U);
#elif defined(_MSC_VER) && defined(_M_X64)
    *a = _umul128(*a, *b, b);
#else
    uint64_t ha = *a >> 32U;
    uint64_t hb = *b >> 32U;
    uint64_t la = static_cast<uint32_t>(*a);
    uint64_t lb = static_cast<uint32_t>(*b);
    uint64_t hi{};
    uint64_t lo{};
    uint64_t rh = ha * hb;
    uint64_t rm0 = ha * lb;
    uint64_t rm1 = hb * la;
    uint64_t rl = la * lb;
    uint64_t t = rl + (rm0 << 32U);
    auto c = static_cast<uint64_t>(t < rl);
    lo = t + (rm1 << 32U);
    c += static_cast<uint64_t>(lo < t);
    hi = rh + (rm0 >> 32U) + (rm1 >> 32U) + c;
    *a = lo;
    *b = hi;
#endif
}

inline uint64_t mix(uint64_t a, uint64_t b) noexcept
{
    mum(&a, &b);
    return a ^ b;
}

inline uint64_t r8(const uint8_t* p) noexcept
{
    uint64_t v{};
    std::memcpy(&v, p, 8U);
    return v;
}

inline uint64_t r4(const uint8_t* p) noexcept
{
    uint32_t v{};
    std::memcpy(&v, p, 4);
    return v;
}

inline uint64_t r3(const uint8_t* p, size_t k) noexcept
{
    return (static_cast<uint64_t>(p[0]) << 16U) | (static_cast<uint64_t>(p[k >> 1U]) << 8U) | p[k - 1];
}

inline uint64_t hash(void const* key, size_t len) noexcept
{
    static constexpr uint64_t secret[] = {0xa0761d6478bd642fULL, 0xe7037ed1a0b428dbULL, 0x8ebc6af09c88c6e3ULL,
                                          0x589965cc75374cc3ULL};

    auto const* p = static_cast<uint8_t const*>(key);
    uint64_t seed = secret[0];
    uint64_t a{};
    uint64_t b{};
    if (len <= 16)
    {
        if (len >= 4)
        {
            a = (r4(p) << 32U) | r4(p + ((len >> 3U) << 2U));
            b = (r4(p + len - 4) << 32U) | r4(p + len - 4 - ((len >> 3U) << 2U));
        }
        else if (len > 0)
        {
            a = r3(p, len);
            b = 0;
        }
        else
        {
            a = 0;
            b = 0;
        }
    }
    else
    {
        size_t i = len;
        if (i > 48)
        {
            uint64_t see1 = seed;
            uint64_t see2 = seed;
            do
            {
                seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
                see1 = mix(r8(p + 16) ^ secret[2], r8(p + 24) ^ see1);
                see2 = mix(r8(p + 32) ^ secret[3], r8(p + 40) ^ see2);
                p += 48;
                i -= 48;
            } while (i > 48);
            seed ^= see1 ^ see2;
        }
        while (i > 16)
        {
            seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
            i -= 16;
            p += 16;
        }
        a = r8(p + i - 16);
        b = r8(p + i - 8);
    }

    return mix(secret[1] ^ len, mix(a ^ secret[1], b ^ seed));
}

inline uint64_t hash(uint64_t x) noexcept
{
    return mix(x, 0x9E3779B97F4A7C15ULL);
}

template <typename T>
struct AlwaysFalse : std::false_type
{};
} // namespace _private

template <typename T>
uint64_t hash(const T& value) noexcept
{
    static_assert(_private::AlwaysFalse<T>::value, "Hash function not provided for this type");
    return 0;
}

template <>
uint64_t hash<uint64_t>(const uint64_t& value) noexcept
{
    return _private::hash(value);
}

template <>
uint64_t hash<uint32_t>(const uint32_t& value) noexcept
{
    return _private::hash(value);
}

template <>
uint64_t hash<uint16_t>(const uint16_t& value) noexcept
{
    return _private::hash(value);
}

template <>
uint64_t hash<uint8_t>(const uint8_t& value) noexcept
{
    return _private::hash(value);
}

template <>
uint64_t hash<int64_t>(const int64_t& value) noexcept
{
    return _private::hash(value);
}

template <>
uint64_t hash<int32_t>(const int32_t& value) noexcept
{
    return _private::hash(value);
}

template <>
uint64_t hash<int16_t>(const int16_t& value) noexcept
{
    return _private::hash(value);
}

template <>
uint64_t hash<int8_t>(const int8_t& value) noexcept
{
    return _private::hash(value);
}

template <>
uint64_t hash<double>(const double& value) noexcept
{
    return _private::hash(*((uint64_t*)&value));
}

template <>
uint64_t hash<float>(const float& value) noexcept
{
    return bul::hash<double>(value);
}

template <typename T>
requires std::is_pointer_v<T>
uint64_t hash(const T& value) noexcept
{
    return _private::hash(uint64_t(value));
}

template <typename T>
requires std::constructible_from<std::string_view, T>
uint64_t hash(const T& value) noexcept
{
    std::string_view string_view = value;
    return _private::hash(string_view.data(), string_view.size());
}

template <typename T>
requires std::is_enum_v<T>
uint64_t hash(const T& value) noexcept
{
    return bul::hash(std::underlying_type_t<T>(value))
}
} // namespace bul
