#include "doctest.h"

#include "bul/containers/pool.h"

TEST_SUITE_BEGIN("pool");

TEST_CASE("simple insert get")
{
    bul::pool<int> pool(1);
    bul::handle<int> h1 = pool.insert(1);
    CHECK(h1.version == 0);
    CHECK(h1.value == 0);
    CHECK(pool.get(h1) == 1);
}

TEST_CASE("multiple insert get")
{
    bul::pool<int> pool(1);
    bul::handle<int> h1 = pool.insert(1);
    bul::handle<int> h2 = pool.insert(2);
    bul::handle<int> h3 = pool.insert(3);
    bul::handle<int> h4 = pool.insert(4);

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

TEST_CASE("simple erase")
{
    bul::pool<int> pool(1);
    bul::handle<int> h1 = pool.insert(1);
    CHECK(pool.is_valid(h1));
    pool.erase(h1);
    CHECK(!pool.is_valid(h1));
    h1 = pool.insert(2);
    CHECK(h1.value == 0);
    CHECK(h1.version == 1);
    CHECK(pool.get(h1) == 2);
}

TEST_CASE("multiple erase")
{
    bul::pool<int> pool(1);
    bul::handle<int> h1 = pool.insert(1);
    bul::handle<int> h2 = pool.insert(2);
    bul::handle<int> h3 = pool.insert(3);
    bul::handle<int> h4 = pool.insert(4);
    pool.erase(h2);
    pool.erase(h4);
    bul::handle<int> h5 = pool.insert(5);
    bul::handle<int> h6 = pool.insert(6);
    pool.erase(h5);
    bul::handle<int> h7 = pool.insert(7);

    CHECK(h6.value == 1);
    CHECK(h6.version == 1);
    CHECK(h7.value == 3);
    CHECK(h7.version == 2);
    CHECK(pool.get(h6) == 6);
    CHECK(pool.get(h7) == 7);
}

TEST_CASE("clear")
{
    bul::pool<int> pool(1);
    pool.insert(1);
    pool.insert(2);
    pool.insert(3);
    pool.insert(4);
    pool.clear();
    CHECK(pool.begin() == pool.end());
}

TEST_CASE("iterator")
{
    bul::pool<int> pool;
    pool.insert(0);
    pool.insert(1);
    pool.insert(2);
    pool.insert(3);
    pool.insert(4);

    int values[] = {0, 1, 2, 3, 4};
    int count = 0;
    for (int i : pool)
    {
        CHECK(i == values[i]);
        ++count;
    }
    CHECK(count == 5);
}

TEST_CASE("iterator erase")
{
    bul::pool<int> pool;
    pool.insert(0);
    pool.insert(1);
    bul::handle<int> h = pool.insert(2);
    pool.insert(3);
    pool.insert(4);

    pool.erase(h);

    int values[] = {0, 1, -1, 3, 4};
    int count = 0;
    for (int i : pool)
    {
        CHECK(i == values[i]);
        ++count;
    }
    CHECK(count == 4);
}

TEST_SUITE_END;