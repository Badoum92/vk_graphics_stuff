#include "doctest.h"

#include <string>

#include "bul/containers/map.h"

TEST_SUITE_BEGIN("map");

TEST_CASE("simple insert")
{
    bul::Map<int, std::string> m;
    CHECK(m.size() == 0);
    auto* entry = m.emplace(1, "1");
    CHECK(entry != nullptr);
    CHECK(entry->key == 1);
    CHECK(entry->val == "1");
    CHECK(m.size() == 1);
}

TEST_CASE("simple lookup")
{
    bul::Map<int, std::string> m;
    auto* entry = m.emplace(1, "1");
    entry = m[1];
    CHECK(entry != nullptr);
    CHECK(entry->key == 1);
    CHECK(entry->val == "1");
    CHECK(m[2] == nullptr);
}

TEST_CASE("grow")
{
    bul::Map<int, std::string> m;
    auto* entry = m.emplace(1, "1");
    CHECK(entry->key == 1);
    CHECK(entry->val == "1");
    CHECK(m.size() == 1);
    CHECK(m.capacity() == 4);

    entry = m.emplace(2, "2");
    CHECK(entry->key == 2);
    CHECK(entry->val == "2");
    CHECK(m.size() == 2);
    CHECK(m.capacity() == 4);

    entry = m.emplace(3, "3");
    CHECK(entry->key == 3);
    CHECK(entry->val == "3");
    CHECK(m.size() == 3);
    CHECK(m.capacity() == 4);

    entry = m.emplace(4, "4");
    CHECK(entry->key == 4);
    CHECK(entry->val == "4");
    CHECK(m.size() == 4);
    CHECK(m.capacity() == 8);

    CHECK(m[1]->val == "1");
    CHECK(m[2]->val == "2");
    CHECK(m[3]->val == "3");
    CHECK(m[4]->val == "4");
}

TEST_CASE("erase")
{
    bul::Map<int, std::string> m;
    m.emplace(1, "1");
    m.emplace(2, "2");
    m.emplace(3, "3");
    m.emplace(4, "4");

    CHECK(!m.erase(5));
    CHECK(m.size() == 4);
    CHECK(m.erase(2));
    CHECK(m.size() == 3);
    CHECK(!m.erase(2));
    CHECK(m.size() == 3);
    CHECK(m[1]->val == "1");
    CHECK(m[2] == nullptr);
    CHECK(m[3]->val == "3");
    CHECK(m[4]->val == "4");
}

TEST_CASE("string key")
{
    bul::Map<std::string, int> m;
    auto* entry = m.emplace("1", 1);
    CHECK(entry->key == "1");
    CHECK(entry->val == 1);
    CHECK(m.size() == 1);
    CHECK(m.capacity() == 4);

    entry = m.emplace("2", 2);
    CHECK(entry->key == "2");
    CHECK(entry->val == 2);
    CHECK(m.size() == 2);
    CHECK(m.capacity() == 4);

    entry = m.emplace("3", 3);
    CHECK(entry->key == "3");
    CHECK(entry->val == 3);
    CHECK(m.size() == 3);
    CHECK(m.capacity() == 4);

    entry = m.emplace("4", 4);
    CHECK(entry->key == "4");
    CHECK(entry->val == 4);
    CHECK(m.size() == 4);
    CHECK(m.capacity() == 8);

    CHECK(m["1"]->val == 1);
    CHECK(m["2"]->val == 2);
    CHECK(m["3"]->val == 3);
    CHECK(m["4"]->val == 4);
}

TEST_SUITE_END();
