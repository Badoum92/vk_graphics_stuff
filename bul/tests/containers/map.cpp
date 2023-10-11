#include "doctest.h"

#include <string>

#include "bul/containers/map.h"

TEST_SUITE_BEGIN("map");

TEST_CASE("simple insert")
{
    bul::map<std::string, std::string> m;
    CHECK(m.size() == 0);
    auto* value = m.emplace("a", "1");
    CHECK(value != nullptr);
    CHECK(*value == "1");
    CHECK(m.size() == 1);
}

TEST_CASE("simple lookup")
{
    bul::map<std::string, std::string> m;
    auto* value = m.emplace("a", "1");
    value = m["a"];
    CHECK(value != nullptr);
    CHECK(*value == "1");
    CHECK(m["b"] == nullptr);
}

TEST_CASE("grow")
{
    bul::map<std::string, std::string> m;
    auto* value = m.emplace("a", "1");
    CHECK(*value == "1");
    CHECK(m.size() == 1);
    CHECK(m.capacity() == 2);

    value = m.emplace("b", "2");
    CHECK(*value == "2");
    CHECK(m.size() == 2);
    CHECK(m.capacity() == 2);

    value = m.emplace("c", "3");
    CHECK(*value == "3");
    CHECK(m.size() == 3);
    CHECK(m.capacity() == 4);

    value = m.emplace("d", "4");
    CHECK(*value == "4");
    CHECK(m.size() == 4);
    CHECK(m.capacity() == 8);

    CHECK(*m["a"] == "1");
    CHECK(*m["b"] == "2");
    CHECK(*m["c"] == "3");
    CHECK(*m["d"] == "4");
}

TEST_CASE("erase")
{
    bul::map<std::string, std::string> m;
    m.emplace("a", "1");
    m.emplace("b", "2");
    m.emplace("c", "3");
    m.emplace("d", "4");

    CHECK(!m.erase("e"));
    CHECK(m.size() == 4);
    CHECK(m.erase("b"));
    CHECK(m.size() == 3);
    CHECK(!m.erase("b"));
    CHECK(m.size() == 3);
    CHECK(*m["a"] == "1");
    CHECK(m["b"] == nullptr);
    CHECK(*m["c"] == "3");
    CHECK(*m["d"] == "4");
}

TEST_CASE("iterator")
{
    bul::map<std::string, std::string> m;
    m.emplace("a", "1");
    m.emplace("b", "2");
    m.emplace("c", "3");
    m.emplace("d", "4");

    size_t i = 0;
    for (const auto& [key, val] : m)
    {
        CHECK(std::string(1, 'a' + i) == *key);
        CHECK(std::string(1, '1' + i) == *val);
        ++i;
    }
}

TEST_SUITE_END;
