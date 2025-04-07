/*! \file unit_test.cpp
 *
 * \brief
 *
 */

#include "storage.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace Sml;

TEST_CASE("StorageBase construct1") {
    auto x = StorageBase<char>();
    CHECK((x) == true);
    CHECK(x.capacity() == default_volume());
    CHECK(x.size() == 0);
}

TEST_CASE("StorageBase construct2") {
    auto x = StorageBase<int>(request_volume(rooms::V128));
    CHECK((x) == true);
    CHECK(x.capacity() == request_volume(rooms::V128));
    CHECK(x.size() == 0);
}

TEST_CASE("StorageBase construct3") {
    auto x = StorageBase<int>(request_volume(rooms::V128), 0xdeadbeef);
    CHECK((x) == true);
    CHECK(x.capacity() == request_volume(rooms::V128));
    CHECK(x.size() == request_volume(rooms::V128));
    CHECK(*x.const_ptr() == 0xdeadbeef);
    // copy construct / copy assign
    SUBCASE("Copy construct/assign") {
        // copy construct
        StorageBase<int>y(x);
        CHECK((x) == true);
        CHECK(y.capacity() == request_volume(rooms::V128));
        CHECK(y.size() == request_volume(rooms::V128));
        CHECK(*y.const_ptr() == 0xdeadbeef);
    }
    // move
    SUBCASE("Move construct") {
        // construct
        StorageBase<int>y(std::move(x));
        CHECK((x) == false);
        CHECK(y.capacity() == request_volume(rooms::V128));
        CHECK(y.size() == request_volume(rooms::V128));
        CHECK(*y.const_ptr() == 0xdeadbeef);
    }
}
struct SomeClass {
    int a = 1000;
    std::string b = {"Hello world"};
};

TEST_CASE("Direct construct") {
    StorageBase<SomeClass> x(4, {10, "DEAD_BEEF"});
    CHECK((x) == true);
    for (auto v : x) {
        CHECK(v.a == 10);
        CHECK(v.b == "DEAD_BEEF");
    }
}

TEST_CASE("Copy assign") {
    // copy assign
    auto x = StorageBase<int>(default_volume(), 0xdeadbeef);
    auto y = StorageBase<int>(default_volume());
    y = x;
    CHECK((x) == true);
    CHECK((y) == true);
    CHECK(y.capacity() == x.capacity());
    CHECK(y.size() == x.size());
    CHECK(*y.const_ptr() == 0xdeadbeef);
}

TEST_CASE("Move assign") {
    // move assign
    auto x = StorageBase<int>(default_volume(), 0xdeadbeef);
    auto y = StorageBase<int>(default_volume());
    y = std::move(x);
    CHECK((x) == false);

    CHECK(y.capacity() == default_volume());
    CHECK(y.size() == default_volume());
    CHECK(*y.const_ptr() == 0xdeadbeef);
}
