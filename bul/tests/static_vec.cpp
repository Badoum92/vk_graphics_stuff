#include "doctest.h"

#include "bul/containers/static_vec.h"

struct TestInt
{
    TestInt()
    {
        val = 42;
    }

    TestInt(int i)
    {
        val = i;
    }

    int val;
};

struct TestIntDestructor
{
    TestIntDestructor()
    {
        val = 42;
    }

    TestIntDestructor(int i)
    {
        val = i;
    }

    ~TestIntDestructor()
    {
        val = 69;
    }

    int val;
};

TEST_SUITE_BEGIN("static_vec");

TEST_CASE("simple valid")
{
    bul::StaticVec<int, 4> a;
    CHECK(a.capacity() == 4);
    CHECK(a.empty());
    CHECK(a.push_back(1));
    REQUIRE(a.size() == 1);
    CHECK(a[0] == 1);
    CHECK(a.back() == 1);
    CHECK(a.pop_back());
    CHECK(a.empty());
}

TEST_CASE("simple invalid")
{
    bul::StaticVec<int, 1> a;
    CHECK(!a.pop_back());
    CHECK(a.size() == 0);
    CHECK(a.push_back(1));
    CHECK(a.size() == 1);
    CHECK(!a.push_back(2));
    CHECK(a.size() == 1);
}

TEST_CASE("check values")
{
    bul::StaticVec<TestInt, 4> a;
    CHECK(a.push_back({1}));
    CHECK(a.emplace_back(2));
    CHECK(a.emplace_back());
    REQUIRE(a.size() == 3);
    CHECK(a[0].val == 1);
    CHECK(a[1].val == 2);
    CHECK(a[2].val == 42);
}

TEST_CASE("check destructor")
{
    bul::StaticVec<TestIntDestructor, 4> a;
    CHECK(a.push_back({1}));
    CHECK(a.emplace_back(2));
    REQUIRE(a.size() == 2);
    CHECK(a[0].val == 1);
    CHECK(a[1].val == 2);
    CHECK(a.pop_back());
    REQUIRE(a.size() == 1);
    CHECK(a[1].val == 69);
}

TEST_CASE("resize")
{
    bul::StaticVec<TestIntDestructor, 4> a;
    CHECK(!a.resize(5));
    CHECK(a.empty());
    CHECK(a.resize(2));
    REQUIRE(a.size() == 2);
    CHECK(a[0].val == 42);
    CHECK(a[1].val == 42);
    CHECK(a.resize(1));
    REQUIRE(a.size() == 1);
    CHECK(a[1].val == 69);
}

TEST_CASE("initializer list")
{
    bul::StaticVec<int, 4> a{1, 2, 3};
    REQUIRE(a.size() == 3);
    CHECK(a[0] == 1);
    CHECK(a[1] == 2);
    CHECK(a[2] == 3);
}

TEST_CASE("begin end")
{
    bul::StaticVec<int, 4> a{1, 2, 3, 4};
    int val = 1;
    for (auto begin = a.begin(); begin < a.end(); ++begin, ++val)
    {
        CHECK(*begin == val);
    }
}

TEST_SUITE_END();
