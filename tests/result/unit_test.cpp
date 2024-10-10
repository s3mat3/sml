/*! \file unit_test.cpp
 *
 * \brief
 *
 */

#include "result.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

using namespace Sml;

TEST_CASE("Result direct construct") {
    auto x = Result<std::string>("Hello world");
    CHECK((x) == true);
    CHECK(x.value() == "Hello world");
}

TEST_CASE("Result copy construct") {
    std::string str = std::string("Hello world");
    auto x = Result<std::string>(str);
    CHECK((x) == true);
    CHECK(x.value() == "Hello world");
}

TEST_CASE("Result move construct") {
    std::string str = std::string("Hello world");
    auto x = Result<std::string>(std::move(str));
    CHECK((x) == true);
    CHECK(x.value() == "Hello world");
}

TEST_CASE("Error copy construct") {
    auto err = error_type(-1000);
    auto x = Result<std::string>(err);
    CHECK((x) == false);
    CHECK(x.error() == -1000);
}

TEST_CASE("Error move construct") {
    auto x = Result<std::string>(error_type(-1000));
    CHECK((x) == false);
    CHECK(x.error() == -1000);
}

auto f_error_result() -> Result<std::string>
{
    return Result<std::string>(error_type(FAIL_CMD));
}
auto f_ok_result() -> Result<std::string>
{
    return Result<std::string>("OK");
}

TEST_CASE("Fail function return") {
    auto x = f_error_result();
    CHECK(x.has_value() == false);
    CHECK(x.error() == FAIL_CMD);
}

TEST_CASE("OK function return") {
    auto x = f_ok_result();
    CHECK(x.has_value() == true);
    CHECK(x.value() == "OK");
}


class SomeClass
{
public:
    explicit SomeClass(int x, int y) : a(x), b(y) {}
    auto str() const noexcept
    {
        std::string msg {"a + b = "};
        msg.append(std::to_string(a + b));
        return msg;
    }
private:
    int a = 0;
    int b = 0;
};

TEST_CASE("class result")
{
    using result = Result<SomeClass>;
    SUBCASE("Direct construct"){
        auto x = result(10, 100);
        REQUIRE((x));
        CHECK(x.value().str() == "a + b = 110");
    }
    SUBCASE("Copy construct"){
        SomeClass s{20, 200};
        auto x = result(s);
        REQUIRE((x));
        CHECK(x.value().str() == "a + b = 220");
    }
    SUBCASE("Move construct"){
        auto x = result(SomeClass{30, 300});
        REQUIRE((x));
        CHECK(x.value().str() == "a + b = 330");
    }
};
