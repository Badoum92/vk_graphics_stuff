#include "doctest.h"

#include "bul/math/vector.h"

TEST_SUITE_BEGIN("vector");

TEST_CASE("default init")
{
    bul::vec2i a;
    CHECK(a.x == 0);
    CHECK(a.y == 0);
    bul::vec3i b;
    CHECK(b.x == 0);
    CHECK(b.y == 0);
    CHECK(b.z == 0);
    bul::vec4i c;
    CHECK(c.x == 0);
    CHECK(c.y == 0);
    CHECK(c.z == 0);
    CHECK(c.w == 0);
}

TEST_CASE("value init")
{
    bul::vec2i a{42};
    CHECK(a.x == 42);
    CHECK(a.y == 42);
    bul::vec3i b{42};
    CHECK(b.x == 42);
    CHECK(b.y == 42);
    CHECK(b.z == 42);
    bul::vec4i c{42};
    CHECK(c.x == 42);
    CHECK(c.y == 42);
    CHECK(c.z == 42);
    CHECK(c.w == 42);
}

TEST_CASE("multiple init")
{
    bul::vec2i a{1, 2};
    CHECK(a.x == 1);
    CHECK(a.y == 2);
    bul::vec3i b{1, 2, 3};
    CHECK(b.x == 1);
    CHECK(b.y == 2);
    CHECK(b.z == 3);
    bul::vec4i c{1, 2, 3, 4};
    CHECK(c.x == 1);
    CHECK(c.y == 2);
    CHECK(c.z == 3);
    CHECK(c.w == 4);
}

TEST_CASE("operator +")
{
    bul::vec2i a = bul::vec2i{1, 2} + bul::vec2i{1, 2};
    CHECK(a.x == 2);
    CHECK(a.y == 4);
    bul::vec3i b = bul::vec3i{1, 2, 3} + bul::vec3i{1, 2, 3};
    CHECK(b.x == 2);
    CHECK(b.y == 4);
    CHECK(b.z == 6);
    bul::vec4i c = bul::vec4i{1, 2, 3, 4} + bul::vec4i{1, 2, 3, 4};
    CHECK(c.x == 2);
    CHECK(c.y == 4);
    CHECK(c.z == 6);
    CHECK(c.w == 8);
}

TEST_CASE("operator -")
{
    bul::vec2i a = bul::vec2i{1, 2} - bul::vec2i{1, 2};
    CHECK(a.x == 0);
    CHECK(a.y == 0);
    bul::vec3i b = bul::vec3i{1, 2, 3} - bul::vec3i{1, 2, 3};
    CHECK(b.x == 0);
    CHECK(b.y == 0);
    CHECK(b.z == 0);
    bul::vec4i c = bul::vec4i{1, 2, 3, 4} - bul::vec4i{1, 2, 3, 4};
    CHECK(c.x == 0);
    CHECK(c.y == 0);
    CHECK(c.z == 0);
    CHECK(c.w == 0);
}

TEST_CASE("operator *")
{
    bul::vec2i a = bul::vec2i{1, 2} * bul::vec2i{1, 2};
    CHECK(a.x == 1);
    CHECK(a.y == 4);
    a = a * 2;
    CHECK(a.x == 2);
    CHECK(a.y == 8);
    bul::vec3i b = bul::vec3i{1, 2, 3} * bul::vec3i{1, 2, 3};
    CHECK(b.x == 1);
    CHECK(b.y == 4);
    CHECK(b.z == 9);
    b = b * 2;
    CHECK(b.x == 2);
    CHECK(b.y == 8);
    CHECK(b.z == 18);
    bul::vec4i c = bul::vec4i{1, 2, 3, 4} * bul::vec4i{1, 2, 3, 4};
    CHECK(c.x == 1);
    CHECK(c.y == 4);
    CHECK(c.z == 9);
    CHECK(c.w == 16);
    c = c * 2;
    CHECK(c.x == 2);
    CHECK(c.y == 8);
    CHECK(c.z == 18);
    CHECK(c.w == 32);
}

TEST_CASE("operator /")
{
    bul::vec2i a = bul::vec2i{8, 8} / bul::vec2i{2, 2};
    CHECK(a.x == 4);
    CHECK(a.y == 4);
    a = a / 2;
    CHECK(a.x == 2);
    CHECK(a.y == 2);
    bul::vec3i b = bul::vec3i{8, 8, 8} / bul::vec3i{2, 2, 2};
    CHECK(b.x == 4);
    CHECK(b.y == 4);
    CHECK(b.z == 4);
    b = b / 2;
    CHECK(b.x == 2);
    CHECK(b.y == 2);
    CHECK(b.z == 2);
    bul::vec4i c = bul::vec4i{8, 8, 8, 8} / bul::vec4i{2, 2, 2, 2};
    CHECK(c.x == 4);
    CHECK(c.y == 4);
    CHECK(c.z == 4);
    CHECK(c.w == 4);
    c = c / 2;
    CHECK(c.x == 2);
    CHECK(c.y == 2);
    CHECK(c.z == 2);
    CHECK(c.w == 2);
}

TEST_CASE("operator +=")
{
    bul::vec2i a;
    a += bul::vec2i{1, 2};
    CHECK(a.x == 1);
    CHECK(a.y == 2);
    bul::vec3i b;
    b += bul::vec3i{1, 2, 3};
    CHECK(b.x == 1);
    CHECK(b.y == 2);
    CHECK(b.z == 3);
    bul::vec4i c;
    c += bul::vec4i{1, 2, 3, 4};
    CHECK(c.x == 1);
    CHECK(c.y == 2);
    CHECK(c.z == 3);
    CHECK(c.w == 4);
}

TEST_CASE("operator -=")
{
    bul::vec2i a;
    a -= bul::vec2i{1, 2};
    CHECK(a.x == -1);
    CHECK(a.y == -2);
    bul::vec3i b;
    b -= bul::vec3i{1, 2, 3};
    CHECK(b.x == -1);
    CHECK(b.y == -2);
    CHECK(b.z == -3);
    bul::vec4i c;
    c -= bul::vec4i{1, 2, 3, 4};
    CHECK(c.x == -1);
    CHECK(c.y == -2);
    CHECK(c.z == -3);
    CHECK(c.w == -4);
}

TEST_CASE("operator *=")
{
    bul::vec2i a{1, 2};
    a *= bul::vec2i{1, 2};
    CHECK(a.x == 1);
    CHECK(a.y == 4);
    a *= 2;
    CHECK(a.x == 2);
    CHECK(a.y == 8);
    bul::vec3i b{1, 2, 3};
    b *= bul::vec3i{1, 2, 3};
    CHECK(b.x == 1);
    CHECK(b.y == 4);
    CHECK(b.z == 9);
    b *= 2;
    CHECK(b.x == 2);
    CHECK(b.y == 8);
    CHECK(b.z == 18);
    bul::vec4i c{1, 2, 3, 4};
    c *= bul::vec4i{1, 2, 3, 4};
    CHECK(c.x == 1);
    CHECK(c.y == 4);
    CHECK(c.z == 9);
    CHECK(c.w == 16);
    c *= 2;
    CHECK(c.x == 2);
    CHECK(c.y == 8);
    CHECK(c.z == 18);
    CHECK(c.w == 32);
}

TEST_CASE("operator /=")
{
    bul::vec2i a{8, 8};
    a /= bul::vec2i{2, 2};
    CHECK(a.x == 4);
    CHECK(a.y == 4);
    a /= 2;
    bul::vec3i b{8, 8, 8};
    b /= bul::vec3i{2, 2, 2};
    CHECK(b.x == 4);
    CHECK(b.y == 4);
    CHECK(b.z == 4);
    b /= 2;
    CHECK(b.x == 2);
    CHECK(b.y == 2);
    CHECK(b.z == 2);
    bul::vec4i c{8, 8, 8, 8};
    c /= bul::vec4i{2, 2, 2, 2};
    CHECK(c.x == 4);
    CHECK(c.y == 4);
    CHECK(c.z == 4);
    CHECK(c.w == 4);
    c /= 2;
    CHECK(c.x == 2);
    CHECK(c.y == 2);
    CHECK(c.z == 2);
    CHECK(c.w == 2);
}

TEST_CASE("operator ==")
{
    CHECK(bul::vec2i{0, 0} == bul::vec2i{0, 0});
    CHECK(bul::vec2i{0, 0} != bul::vec2i{1, 1});
    CHECK(bul::vec3i{0, 0, 0} == bul::vec3i{0, 0, 0});
    CHECK(bul::vec3i{0, 0, 0} != bul::vec3i{1, 1, 1});
    CHECK(bul::vec4i{0, 0, 0, 0} == bul::vec4i{0, 0, 0, 0});
    CHECK(bul::vec4i{0, 0, 0, 0} != bul::vec4i{1, 1, 1, 1});
}

TEST_CASE("min")
{
    CHECK(bul::min(bul::vec4f{3, 4, 1, 2}) == 1);
    CHECK(bul::min_comp(bul::vec4f{3, 4, 1, 2}) == 2);
}

TEST_CASE("max")
{
    CHECK(bul::max(bul::vec4f{3, 4, 1, 2}) == 4);
    CHECK(bul::max_comp(bul::vec4f{3, 4, 1, 2}) == 1);
}

TEST_CASE("dot")
{
    CHECK(bul::dot(bul::vec4f{1, 2, 3, 4}, bul::vec4f{5, 6, 7, 8}) == 70);
}

TEST_CASE("length")
{
    CHECK(bul::length(bul::vec4f{0, 0, 0, 0}) == 0);
    CHECK(bul::length(bul::vec4f{1, 0, 0, 0}) == 1);
    CHECK(bul::length(bul::vec4f{0, 2, 0, 0}) == 2);
    CHECK(bul::length(bul::vec4f{0, 0, 3, 0}) == 3);
    CHECK(bul::length(bul::vec4f{0, 0, 0, 4}) == 4);
}

TEST_CASE("distance")
{
    CHECK(bul::distance(bul::vec4f{0, 0, 0, 0}, bul::vec4f{1, 0, 0, 0}) == 1);
    CHECK(bul::distance(bul::vec4f{1, 0, 0, 0}, bul::vec4f{2, 0, 0, 0}) == 1);
    CHECK(bul::distance(bul::vec4f{1, 1, 0, 0}, bul::vec4f{1, 2, 0, 0}) == 1);
}

TEST_CASE("normalize")
{
    CHECK(bul::length(bul::normalize(bul::vec4f{1, 2, 3, 4})) == doctest::Approx(1.0f));
}

TEST_SUITE_END();
