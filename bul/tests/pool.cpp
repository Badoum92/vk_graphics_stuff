#include "doctest.h"

#include "bul/containers/pool.h"

TEST_SUITE_BEGIN("pool");

TEST_CASE("simple insert get")
{
    bul::Pool<int, 1> pool;
    bul::Handle<int> h1 = pool.insert(1);
    CHECK(h1.version == 0);
    CHECK(h1.value == 0);
    CHECK(pool.get(h1) == 1);
}

TEST_CASE("multiple insert get")
{
    bul::Pool<int, 1> pool;
    bul::Handle<int> h1 = pool.insert(1);
    bul::Handle<int> h2 = pool.insert(2);
    bul::Handle<int> h3 = pool.insert(3);
    bul::Handle<int> h4 = pool.insert(4);

    CHECK(h1.version == 0);
    CHECK(h1.value == 0);
    CHECK(pool.get(h1) == 1);
    CHECK(h2.version == 0);
    CHECK(h2.value == 1);
    CHECK(pool.get(h2) == 2);
    CHECK(h3.version == 0);
    CHECK(h3.value == 2);
    CHECK(pool.get(h3) == 3);
    CHECK(h4.version == 0);
    CHECK(h4.value == 3);
    CHECK(pool.get(h4) == 4);
}

TEST_CASE("simple remove")
{
    bul::Pool<int, 1> pool;
    bul::Handle<int> h1 = pool.insert(1);
    CHECK(pool.remove(h1));
    CHECK(!pool.remove(h1));
    CHECK(!pool.is_valid(h1));
    h1 = pool.insert(2);
    CHECK(h1.version == 1);
    CHECK(pool.get(h1) == 2);
}

TEST_SUITE_END();
