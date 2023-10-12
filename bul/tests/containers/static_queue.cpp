#include "doctest.h"

#include <string>

#include "bul/containers/static_queue.h"

TEST_SUITE_BEGIN("static_queue");

TEST_CASE("simple valid")
{
    bul::static_queue<std::string, 4> q;
    CHECK(q.capacity() == 4);
    CHECK(q.empty());
    CHECK(q.emplace("1"));
    CHECK(q.size() == 1);
    CHECK(q.front() == "1");
    CHECK(q.pop());
    CHECK(q.empty());
}

TEST_CASE("full")
{
    bul::static_queue<std::string, 4> q;
    CHECK(q.emplace("0"));
    CHECK(q.emplace("1"));
    CHECK(q.emplace("2"));
    CHECK(q.emplace("3"));
    CHECK(q.size() == 4);
    CHECK(!q.emplace("4"));
    CHECK(q.size() == 4);
}

TEST_CASE("push pop")
{
    bul::static_queue<std::string, 4> q;
    CHECK(q.emplace("0"));
    CHECK(q.front() == "0");
    CHECK(q.emplace("1"));
    CHECK(q.front() == "0");
    CHECK(q.emplace("2"));
    CHECK(q.front() == "0");
    CHECK(q.emplace("3"));
    CHECK(q.front() == "0");
    CHECK(q.pop());
    CHECK(q.front() == "1");
    CHECK(q.pop());
    CHECK(q.front() == "2");
    CHECK(q.emplace("4"));
    CHECK(q.emplace("5"));
    CHECK(q.size() == 4);
}

TEST_SUITE_END;
