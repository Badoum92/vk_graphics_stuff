#include "doctest.h"

#include "common_test.h"
#include "bul/containers/static_vector.h"

TEST_SUITE_BEGIN("static_vector");

TEST_CASE("default init")
{
    bul::static_vector<test_struct, 4> v;
    CHECK(v.size() == 0);
    CHECK(v.capacity() == 4);
}

TEST_CASE("default value init")
{
    bul::static_vector<test_struct, 4> v(42);
    CHECK(v.size() == 4);
    CHECK(v.capacity() == 4);
    for (size_t i = 0; i < 4; ++i)
    {
        CHECK(v[i] == 42);
        CHECK(v[i].copy_constructed());
    }
}

TEST_CASE("intializer list")
{
    bul::static_vector<test_struct, 4> v{0, 1, 2, 3};
    CHECK(v.size() == 4);
    CHECK(v.capacity() == 4);
    for (size_t i = 0; i < 4; ++i)
    {
        CHECK(v[i] == i);
        CHECK(v[i].copy_constructed());
    }
}

TEST_CASE("resize")
{
    bul::static_vector<test_struct, 4> v;
    v.resize(4);
    CHECK(v.size() == 4);
    CHECK(v.capacity() == 4);
    for (size_t i = 0; i < 4; ++i)
    {
        CHECK(v[i] == 0);
        CHECK(v[i].default_constructed());
    }
}

TEST_CASE("clear")
{
    bul::static_vector<test_struct, 4> v;
    v.resize(4);
    v.clear();
    CHECK(v.size() == 0);
    CHECK(v.capacity() == 4);
    CHECK((v._begin + 0)->destroyed());
    CHECK((v._begin + 1)->destroyed());
    CHECK((v._begin + 2)->destroyed());
    CHECK((v._begin + 3)->destroyed());
}

TEST_CASE("emplace back copy")
{
    bul::static_vector<test_struct, 4> v;
    test_struct t0 = 0;
    test_struct t1 = 1;
    test_struct t2 = 2;
    test_struct t3 = 3;

    v.emplace_back(t0);
    CHECK(v.size() == 1);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);

    v.emplace_back(t1);
    CHECK(v.size() == 2);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[1] == 1);

    v.emplace_back(t2);
    CHECK(v.size() == 3);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[1] == 1);
    CHECK(v[2] == 2);

    v.emplace_back(t3);
    CHECK(v.size() == 4);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[1] == 1);
    CHECK(v[2] == 2);
    CHECK(v[3] == 3);
}

TEST_CASE("emplace back move")
{
    bul::static_vector<test_struct, 4> v;
    test_struct t0 = 0;
    test_struct t1 = 1;
    test_struct t2 = 2;
    test_struct t3 = 3;

    v.emplace_back(std::move(t0));
    CHECK(v.size() == 1);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);

    v.emplace_back(std::move(t1));
    CHECK(v.size() == 2);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[1] == 1);

    v.emplace_back(std::move(t2));
    CHECK(v.size() == 3);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[1] == 1);
    CHECK(v[2] == 2);

    v.emplace_back(std::move(t3));
    CHECK(v.size() == 4);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[1] == 1);
    CHECK(v[2] == 2);
    CHECK(v[3] == 3);
}

TEST_CASE("emplace back in place")
{
    bul::static_vector<test_struct, 4> v;

    v.emplace_back(0);
    CHECK(v.size() == 1);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);

    v.emplace_back(1);
    CHECK(v.size() == 2);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[1] == 1);

    v.emplace_back(2);
    CHECK(v.size() == 3);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[1] == 1);
    CHECK(v[2] == 2);

    v.emplace_back(3);
    CHECK(v.size() == 4);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[1] == 1);
    CHECK(v[2] == 2);
    CHECK(v[3] == 3);
}

TEST_CASE("pop back")
{
    bul::static_vector<test_struct, 4> v{0, 1, 2, 3};

    CHECK(v.pop_back() == 3);
    CHECK(v.size() == 3);
    CHECK(v.capacity() == 4);

    CHECK(v.pop_back() == 2);
    CHECK(v.size() == 2);
    CHECK(v.capacity() == 4);

    CHECK(v.pop_back() == 1);
    CHECK(v.size() == 1);
    CHECK(v.capacity() == 4);

    CHECK(v.pop_back() == 0);
    CHECK(v.size() == 0);
    CHECK(v.capacity() == 4);
}

TEST_CASE("pop back emplace back")
{
    bul::static_vector<test_struct, 4> v{0, 1, 2, 3};

    CHECK(v.pop_back() == 3);
    CHECK(v.size() == 3);
    CHECK(v.capacity() == 4);

    v.emplace_back(4);

    CHECK(v.pop_back() == 4);
    CHECK(v.size() == 3);
    CHECK(v.capacity() == 4);
}

TEST_CASE("front back")
{
    bul::static_vector<test_struct, 4> v{0, 1, 2, 3};
    CHECK(v.front() == 0);
    CHECK(v.back() == 3);
}

TEST_CASE("begin end")
{
    bul::static_vector<test_struct, 4> v{0, 1, 2, 3};
    CHECK(*v.begin() == 0);
    CHECK(*(v.end() - 1) == 3);
}

TEST_CASE("find")
{
    bul::static_vector<test_struct, 4> v{0, 1, 2, 3};
    CHECK(v.find(0) == 0);
    CHECK(v.find(1) == 1);
    CHECK(v.find(2) == 2);
    CHECK(v.find(3) == 3);
}

TEST_CASE("erase stable")
{
    bul::static_vector<test_struct, 4> v{0, 1, 2, 3};

    v.erase_stable(1);
    CHECK(v.size() == 3);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[0].copy_constructed());
    CHECK(v[1] == 2);
    CHECK(v[1].move_constructed());
    CHECK(v[2] == 3);
    CHECK(v[2].move_constructed());
    CHECK((v._begin + 3)->destroyed());

    v.erase_stable(0);
    CHECK(v.size() == 2);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 2);
    CHECK(v[0].move_constructed());
    CHECK(v[1] == 3);
    CHECK(v[1].move_constructed());
    CHECK((v._begin + 2)->destroyed());
    CHECK((v._begin + 3)->destroyed());

    v.erase_stable(0);
    v.erase_stable(0);

    CHECK(v.size() == 0);
    CHECK(v.capacity() == 4);
    CHECK((v._begin + 0)->destroyed());
    CHECK((v._begin + 1)->destroyed());
    CHECK((v._begin + 2)->destroyed());
    CHECK((v._begin + 3)->destroyed());
}

TEST_CASE("erase")
{
    bul::static_vector<test_struct, 4> v{0, 1, 2, 3};

    v.erase(1);
    CHECK(v.size() == 3);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 0);
    CHECK(v[0].copy_constructed());
    CHECK(v[1] == 3);
    CHECK(v[1].move_constructed());
    CHECK(v[2] == 2);
    CHECK(v[2].copy_constructed());
    CHECK((v._begin + 3)->destroyed());

    v.erase(0);
    CHECK(v.size() == 2);
    CHECK(v.capacity() == 4);
    CHECK(v[0] == 2);
    CHECK(v[0].move_constructed());
    CHECK(v[1] == 3);
    CHECK(v[1].move_constructed());
    CHECK((v._begin + 2)->destroyed());
    CHECK((v._begin + 3)->destroyed());

    v.erase(0);
    v.erase(0);

    CHECK(v.size() == 0);
    CHECK(v.capacity() == 4);
    CHECK((v._begin + 0)->destroyed());
    CHECK((v._begin + 1)->destroyed());
    CHECK((v._begin + 2)->destroyed());
    CHECK((v._begin + 3)->destroyed());
}

TEST_CASE("destroyed")
{
    bul::static_vector<test_struct, 4> v{0, 1, 2, 3};
    v.~static_vector();
    CHECK(v.size() == 0);
    CHECK(v.capacity() == 4);
}

TEST_CASE("automatic loop")
{
    bul::static_vector<test_struct, 4> v{0, 1, 2, 3};
    int i = 0;
    for (const auto& element : v)
    {
        CHECK(element == i);
        ++i;
    }
}

TEST_CASE("copy")
{
    bul::static_vector<test_struct, 4> v1{0, 1, 2, 3};
    bul::static_vector<test_struct, 4> v2 = v1;

    CHECK(v1.size() == 4);
    CHECK(v1.capacity() == 4);
    CHECK(v1[0] == 0);
    CHECK(v1[1] == 1);
    CHECK(v1[2] == 2);
    CHECK(v1[3] == 3);

    CHECK(v2.size() == 4);
    CHECK(v2.capacity() == 4);
    CHECK(v2[0] == 0);
    CHECK(v2[1] == 1);
    CHECK(v2[2] == 2);
    CHECK(v2[3] == 3);
}

TEST_CASE("move")
{
    bul::static_vector<test_struct, 4> v1{0, 1, 2, 3};
    bul::static_vector<test_struct, 4> v2 = std::move(v1);

    CHECK(v1.size() == 0);
    CHECK(v1.capacity() == 4);

    CHECK(v2.size() == 4);
    CHECK(v2.capacity() == 4);
    CHECK(v2[0] == 0);
    CHECK(v2[1] == 1);
    CHECK(v2[2] == 2);
    CHECK(v2[3] == 3);
}

TEST_SUITE_END;
