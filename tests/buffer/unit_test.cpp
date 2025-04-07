/**
 * @file unit_test.cpp
 *
 * @copyright © 2025 s3mat3
 *
 * This code is licensed under the MIT License, see the LICENSE.txt file for details
 *
 * @brief Unit test for buffer class
 *
 * @author s3mat3
 */

//#undef TRACE_FUNCTION
#include "byte_buffer.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS
#include "doctest.h"


using namespace Sml;
struct Some {
    Some() : Some(0, 0) {}
    Some(int x, int y) : x{x}, y{y} {}
    ~Some() = default;
    int x;
    int y;
};

TEST_CASE("BufferBase construct1") {
    auto x = ByteBuffer(8);
    CHECK((x) == true);
    CHECK(x.capacity() == 8);
    CHECK(x.size() == 0);
}

TEST_CASE("BufferBase construct2") {
    auto x = BufferBase<Some>(8);
    CHECK((x) == true);
    CHECK(x.capacity() == 8);
    CHECK(x.size() == 0);
    Some s1;
    x.push_back(s1);
    s1.x = 1, s1.y = -1;
    x.push_back(s1);
    CHECK(x.size() == 2);
}

TEST_CASE("BufferBase construct3") {
    auto x = BufferBase<Some>(8, {100,-100});
    REQUIRE((x) == true);
    CHECK(x.capacity() == 8);
    for (auto a : x) {
        CHECK(a.x == 100);
        CHECK(a.y == -100);
    }
}

static constexpr size_type TEST_SIZE = 8;
static constexpr char ar[TEST_SIZE] = {0x0,0x1,0x2,0x3,0x4,0x5,0x6,0x7};
TEST_CASE("BufferBase basic") {
    auto x = ByteBuffer(TEST_SIZE);
    auto sz = x.capacity();
    CHECK(x.size() == 0);
    REQUIRE(x.capacity() == TEST_SIZE);
    for (size_type i = 0; i < sz; ++i) {
        x.push_back(ar[i]);
    }
    CHECK(x.size() == 8);
    // for (size_type i = 0; i < sz; ++i) {
    //     CHECK(x.at(i) == ar[i]);
    // }
    SUBCASE("Check at function") {
        CHECK(x.size() == 8);
        for (size_type i = 0; i < sz; ++i) {
             CHECK(x.at(i) == ar[i]);
        }
        REQUIRE_THROWS_AS(x.at(9), std::out_of_range);
    }
    SUBCASE("Check operator []") {
        CHECK(x.size() == TEST_SIZE);
        for (size_type i = 0; i < sz; ++i) {
            CHECK(x[i] == ar[i]);
        }
        REQUIRE_THROWS_AS(x[TEST_SIZE + 1], std::out_of_range);
        x[0] = 0x7f;
        CHECK(x[0] == 0x7f);
    }
}

TEST_CASE("Operator +") {
    auto x = ByteBuffer(TEST_SIZE);
    auto sz = x.capacity();
    CHECK(x.size() == 0);
    REQUIRE(x.capacity() == TEST_SIZE);
    for (size_type i = 0; i < sz; ++i) {
        x = x + ar[i];
    }
    for (size_type i = 0; i < sz; ++i) {
        CHECK(x[i] == ar[i]);
    }
}

const char* ptr = "Hello";
TEST_CASE("BufferBase append") {
    auto x = ByteBuffer(TEST_SIZE);
    auto sz = x.capacity();
    for (size_type i = 0; i < sz; ++i) {
        x.push_back(ar[i]);
    }
    SUBCASE("append function from value_type*") {
        auto y = ByteBuffer(TEST_SIZE * 2);
        REQUIRE(y.capacity() == 16);
        y.append(ptr, 5);
        CHECK(y[0] == 'H');
        CHECK(y[2] == 'l');
        CHECK(y[4] == 'o');
    }
    SUBCASE("assign function from same type") {
        auto y = ByteBuffer(TEST_SIZE * 3);
        REQUIRE(y.capacity() == 24);
        y.append(x);
        for (size_type i = 0; i < TEST_SIZE; ++i) {
            CHECK(y[i] == x[i]);
        }
        y.append(ptr, 5);
        CHECK(y.size() == 13);
        CHECK(y[8] == 'H');
        CHECK(y[10] == 'l');
        CHECK(y[12] == 'o');
    }
}

TEST_CASE("BufferBase assign") {
    auto x = ByteBuffer(TEST_SIZE);
    auto sz = x.capacity();
    for (size_type i = 0; i < sz; ++i) {
        x.push_back(ar[i]);
    }
    SUBCASE("assign function from value_type*") {
        auto y = ByteBuffer(TEST_SIZE);
        y.assign(ptr, 5);
        REQUIRE(y.size() == 5);
        CHECK(y[0] == 'H');
        CHECK(y[2] == 'l');
        CHECK(y[4] == 'o');
    }
    SUBCASE("assign function from same type") {
        auto y = ByteBuffer(TEST_SIZE);
        REQUIRE(x.capacity() == y.capacity());
        y.assign(x);
        REQUIRE(x.size() == TEST_SIZE);
        for (size_type i = 0; i < sz; ++i) {
            CHECK(y[i] == x[i]);
        }
        REQUIRE(x.size() == y.size());
    }
}

TEST_CASE("extract OK result") {
    const char* c = "Hello world!!";
    auto x = ByteBuffer(16);
    x.assign(c, 13);
    CHECK(x.size() == 13);
    auto y = x.extract(6, 5);
    CHECK((y) == true);
    CHECK(y.value()[0] == 'w');
    CHECK(y.value()[1] == 'o');
    CHECK(y.value()[2] == 'r');
    CHECK(y.value()[3] == 'l');
    CHECK(y.value()[4] == 'd');
}

TEST_CASE("extract NG result") {
    const char* c = "Hello world!!";
    auto x = ByteBuffer(16);
    x.assign(c, 13);
    CHECK(x.size() == 13);
    auto y = x.extract(6, 7);
    REQUIRE((y) == true);
    CHECK(y.value()[5] == '!');
    CHECK(y.value()[6] == '!');
    auto z = x.extract(6, 8);
    REQUIRE((z) == false);
    CHECK(z.error() == OUT_OF_RANGE);
}

TEST_CASE("read") {
    const char* c = "Hello world!!";
    auto x = ByteBuffer(16);
    x.assign(c, 13);
    CHECK(x.read() == 'H');
    CHECK(x.read() == 'e');
    CHECK(x.read() == 'l');
    CHECK(x.read() == 'l');
    CHECK(x.read() == 'o');
    x.put_back();
    CHECK(x.read() == 'o');
    x.position(12);
    CHECK(x.read() == '!');
    REQUIRE_THROWS_AS(x.read(), std::out_of_range);
}

std::string str("Hello world");
std::string rts("Wellcaome to the hell");

TEST_CASE("ByteBuffer helper") {
    auto x = from_string(rts);
    CHECK(x.capacity() == rts.size());
    CHECK(x[0] == 'W');
    CHECK(x[rts.size()-1] == 'l');
    auto y = to_string(x);
    CHECK(y == rts);
}
