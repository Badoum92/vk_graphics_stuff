#include "doctest.h"

#include "bul/containers/static_queue.h"

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

    int val;
};

TEST_SUITE_BEGIN("static_queue");

TEST_CASE("simple valid")
{
    bul::StaticQueue<int, 4> q;
    CHECK(q.capacity() == 4);
    CHECK(q.empty());
    CHECK(q.push_back(1));
    CHECK(q.size() == 1);
    CHECK(q.front() == 1);
    CHECK(q.pop_front());
    CHECK(q.empty());
}

TEST_CASE("full")
{
    bul::StaticQueue<int, 4> q;
    CHECK(q.push_back(0));
    CHECK(q.push_back(1));
    CHECK(q.push_back(2));
    CHECK(q.push_back(3));
    CHECK(q.size() == 4);
    CHECK(!q.push_back(4));
    CHECK(q.size() == 4);
}

TEST_CASE("push pop")
{
    bul::StaticQueue<int, 4> q;
    CHECK(q.push_back(0));
    CHECK(q.front() == 0);
    CHECK(q.push_back(1));
    CHECK(q.front() == 0);
    CHECK(q.push_back(2));
    CHECK(q.front() == 0);
    CHECK(q.push_back(3));
    CHECK(q.front() == 0);
    CHECK(q.pop_front());
    CHECK(q.front() == 1);
    CHECK(q.pop_front());
    CHECK(q.front() == 2);
    CHECK(q.push_back(4));
    CHECK(q.push_back(5));
    CHECK(q.size() == 4);
}

TEST_CASE("iterator")
{
    bul::StaticQueue<int, 4> q;
    CHECK(q.push_back(0));
    CHECK(q.push_back(1));
    CHECK(q.push_back(2));
    CHECK(q.push_back(3));

    int values[] = {0, 1, 2, 3};
    for (int i : q)
    {
        CHECK(i == values[i]);
    }
}

TEST_SUITE_END;
