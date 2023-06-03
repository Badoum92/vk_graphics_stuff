#include "doctest.h"

#include <cstring>

#include "bul/math/matrix.h"

TEST_SUITE_BEGIN("matrix");

// clang-format off
static bul::mat4f values{{
    1, 5, 9, 13,
    2, 6, 10, 14,
    3, 7, 11, 15,
    4, 8, 12, 16
}};

static bul::mat4f identity{{
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
}};
// clang-format on

TEST_CASE("default init")
{
    bul::mat4f m;
    const float zero[16] = {0};
    CHECK(memcmp(m.data, zero, 16 * sizeof(float)) == 0);
}

TEST_CASE("operator ==")
{
    bul::mat4f m = bul::mat4f::identity();
    CHECK(identity == bul::mat4f::identity());
    CHECK(bul::mat4f{values} == values);
}

TEST_CASE("operator !=")
{
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
    bul::mat4f m{values};
    m -= bul::mat4f{identity};
    CHECK(m == bul::mat4f{expected});
}

TEST_CASE("operator *=")
{
    const float expected[16] = {2, 10, 18, 26, 4, 12, 20, 28, 6, 14, 22, 30, 8, 16, 24, 32};
    bul::mat4f m{values};
    m *= 2;
    CHECK(m == bul::mat4f{expected});
}

TEST_CASE("inverse")
{
    const float a[16] = {2, -2, 1, -1, 5, -3, 3, -6, -3, 2, -2, 4, -2, -5, 2, 3};
    bul::mat4f m = bul::inverse(bul::mat4f{a});
    CHECK(m[0][0] == doctest::Approx(0.0f));
    CHECK(m[0][1] == doctest::Approx(2.0f));
    CHECK(m[0][2] == doctest::Approx(3.0f));
    CHECK(m[0][3] == doctest::Approx(0.0f));
    CHECK(m[1][0] == doctest::Approx(-7.0f / 4.0f));
    CHECK(m[1][1] == doctest::Approx(17.0f / 4.0f));
    CHECK(m[1][2] == doctest::Approx(23.0f / 4.0f));
    CHECK(m[1][3] == doctest::Approx(1.0f / 4.0f));
    CHECK(m[2][0] == doctest::Approx(-13.0f / 4.0f));
    CHECK(m[2][1] == doctest::Approx(31.0f / 4.0f));
    CHECK(m[2][2] == doctest::Approx(41.0f / 4.0f));
    CHECK(m[2][3] == doctest::Approx(3.0f / 4.0f));
    CHECK(m[3][0] == doctest::Approx(-3.0f / 4.0f));
    CHECK(m[3][1] == doctest::Approx(13.0f / 4.0f));
    CHECK(m[3][2] == doctest::Approx(19.0f / 4.0f));
    CHECK(m[3][3] == doctest::Approx(1.0f / 4.0f));
}

TEST_SUITE_END();
