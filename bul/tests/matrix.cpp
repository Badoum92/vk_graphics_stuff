#include "doctest.h"

#include <cstring>

#include "bul/math/matrix.h"

TEST_SUITE_BEGIN("matrix");

// clang-format off
static float zero[16] = {0};

static float values[16] = {
    1, 5, 9, 13,
    2, 6, 10, 14,
    3, 7, 11, 15,
    4, 8, 12, 16
};

static float identity[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};
// clang-format on

TEST_CASE("default init")
{
    bul::mat4f m;
    CHECK(memcmp(m.data, zero, 16 * sizeof(float)) == 0);
}

TEST_CASE("value init")
{
    bul::mat4f m{1};
    CHECK(memcmp(m.data, identity, 16 * sizeof(float)) == 0);
}

TEST_CASE("full init")
{
    bul::mat4f m{values};
    CHECK(memcmp(m.data, values, 16 * sizeof(float)) == 0);
}

TEST_CASE("identity")
{
    bul::mat4f m = bul::mat4f::identity();
    CHECK(memcmp(m.data, identity, 16 * sizeof(float)) == 0);
}

TEST_CASE("operator ==")
{
    CHECK(bul::mat4f{values} == bul::mat4f{values});
    CHECK(bul::mat4f{values} != bul::mat4f{identity});
}

TEST_CASE("operator +")
{
    const float expected[16] = {2, 5, 9, 13, 2, 7, 10, 14, 3, 7, 12, 15, 4, 8, 12, 17};
    bul::mat4f m = bul::mat4f{values} + bul::mat4f{identity};
    CHECK(m == bul::mat4f{expected});
}

TEST_CASE("operator -")
{
    const float expected[16] = {0, 5, 9, 13, 2, 5, 10, 14, 3, 7, 10, 15, 4, 8, 12, 15};
    bul::mat4f m = bul::mat4f{values} - bul::mat4f{identity};
    CHECK(m == bul::mat4f{expected});
}

TEST_CASE("operator * float")
{
    const float expected[16] = {2, 10, 18, 26, 4, 12, 20, 28, 6, 14, 22, 30, 8, 16, 24, 32};
    bul::mat4f m = bul::mat4f{values} * 2;
    CHECK(m == bul::mat4f{expected});
}

TEST_CASE("operator * vec4f")
{
    const bul::vec4f expected{30, 70, 110, 150};
    bul::vec4f v = bul::mat4f{values} * bul::vec4f{1, 2, 3, 4};
    CHECK(v == expected);
}

TEST_CASE("operator * mat4f")
{
    const float expected[16] = {90, 202, 314, 426, 100, 228, 356, 484, 110, 254, 398, 542, 120, 280, 440, 600};
    bul::mat4f m = bul::mat4f{values} * bul::mat4f{values};
    CHECK(m == bul::mat4f{expected});
}

TEST_CASE("operator +=")
{
    const float expected[16] = {2, 5, 9, 13, 2, 7, 10, 14, 3, 7, 12, 15, 4, 8, 12, 17};
    bul::mat4f m{values};
    m += bul::mat4f{identity};
    CHECK(m == bul::mat4f{expected});
}

TEST_CASE("operator -=")
{
    const float expected[16] = {0, 5, 9, 13, 2, 5, 10, 14, 3, 7, 10, 15, 4, 8, 12, 15};
    bul::mat4f m = bul::mat4f{values};
    m -= bul::mat4f{identity};
    CHECK(m == bul::mat4f{expected});
}

TEST_CASE("operator *=")
{
    const float expected[16] = {2, 10, 18, 26, 4, 12, 20, 28, 6, 14, 22, 30, 8, 16, 24, 32};
    bul::mat4f m = bul::mat4f{values};
    m *= 2;
    CHECK(m == bul::mat4f{expected});
}

TEST_SUITE_END();
