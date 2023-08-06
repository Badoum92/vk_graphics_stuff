#include "doctest.h"

#include "bul/containers/static_vector.h"

struct TestInt
{
    TestInt()
    {
        val = 42;
    }

    TestInt(const TestInt& other)
    {
        val = other.val;
    }

    TestInt(TestInt&& other)
    {
        val = other.val;
        other.val = 0;
    }

    TestInt(int i)
    {
        val = i;
    }

    ~TestInt()
    {
        val = -1;
    }

    bool operator==(int i) const
    {
        return val == i;
    }

    int val;
};

TEST_SUITE_BEGIN("static_vector");

TEST_CASE("simple valid")
{
    bul::StaticVector<int, 4> a;
    CHECK(a.capacity() == 4);
    CHECK(a.empty());
    CHECK(a.push_back(1) == 1);
    REQUIRE(a.size() == 1);
    CHECK(a[0] == 1);
    CHECK(a.back() == 1);
    CHECK(a.pop_back());
    CHECK(a.empty());
}

TEST_CASE("simple invalid")
{
    bul::StaticVector<int, 1> a;
    CHECK(!a.pop_back());
    CHECK(a.size() == 0);
    CHECK(a.push_back(1) == 1);
    CHECK(a.size() == 1);
}

TEST_CASE("check values")
{
    bul::StaticVector<TestInt, 4> a;
    CHECK(a.push_back({1}) == 1);
    CHECK(a.emplace_back(2) == 2);
    CHECK(a.emplace_back() == 42);
    REQUIRE(a.size() == 3);
    CHECK(a[0] == 1);
    CHECK(a[1] == 2);
    CHECK(a[2] == 42);
}

TEST_CASE("check destructor")
{
    bul::StaticVector<TestInt, 4> a;
    CHECK(a.push_back({1}) == 1);
    CHECK(a.emplace_back(2) == 2);
    REQUIRE(a.size() == 2);
    CHECK(a[0] == 1);
    CHECK(a[1] == 2);
    CHECK(a.pop_back());
    REQUIRE(a.size() == 1);
    CHECK(a[1] == -1);
}

TEST_CASE("resize")
{
    bul::StaticVector<TestInt, 4> a;
    CHECK(!a.resize(5));
    CHECK(a.empty());
    CHECK(a.resize(2));
    REQUIRE(a.size() == 2);
    CHECK(a[0] == 42);
    CHECK(a[1] == 42);
    CHECK(a.resize(1));
    REQUIRE(a.size() == 1);
    CHECK(a[1] == -1);
}

TEST_CASE("initializer list")
{
    bul::StaticVector<int, 4> a{1, 2, 3};
    REQUIRE(a.size() == 3);
    CHECK(a[0] == 1);
    CHECK(a[1] == 2);
    CHECK(a[2] == 3);
}

TEST_CASE("begin end")
{
    bul::StaticVector<int, 4> a{1, 2, 3, 4};
    int val = 1;
    for (auto begin = a.begin(); begin < a.end(); ++begin, ++val)
    {
        CHECK(*begin == val);
    }
}

TEST_CASE("element copy")
{
    bul::StaticVector<TestInt, 4> a;
    TestInt test_int;
    a.push_back(test_int);
    REQUIRE(a.size() == 1);
    CHECK(a[0] == 42);
}

TEST_CASE("element move")
{
    bul::StaticVector<TestInt, 4> a;
    TestInt test_int;
    a.push_back(std::move(test_int));
    REQUIRE(a.size() == 1);
    CHECK(a[0] == 42);
    CHECK(test_int == 0);
}

TEST_CASE("vector copy")
{
    bul::StaticVector<int, 4> a;
    a.push_back(42);
    auto b = a;
    REQUIRE(b.size() == 1);
    REQUIRE(b.capacity() == 4);
    CHECK(b[0] == 42);
}

TEST_CASE("vector move")
{
    bul::StaticVector<int, 4> a;
    a.push_back(42);
    auto b = std::move(a);
    REQUIRE(b.size() == 1);
    REQUIRE(b.capacity() == 4);
    CHECK(b[0] == 42);
}

TEST_CASE("copy")
{
    bul::StaticVector<int, 4> a;
    a.push_back(0);
    {
        bul::StaticVector<int, 4> b = a;
        CHECK(b.capacity() == 4);
        CHECK(b.size() == 1);
        CHECK(b[0] == 0);
        b[0] = 1;
        CHECK(a[0] == 0);
    }
    CHECK(a[0] == 0);
}

TEST_CASE("move")
{
    bul::StaticVector<int, 4> a;
    a.push_back(0);
    {
        bul::StaticVector<int, 4> b = std::move(a);
        CHECK(b.capacity() == 4);
        CHECK(b.size() == 1);
        CHECK(b[0] == 0);
        b[0] = 1;
    }
}

TEST_SUITE_END();
