/**
 * @file unit_test.cpp
 *
 * @copyright © 2023 s3mat3 
 *
 * This code is licensed under the MIT License, see the LICENSE.txt file for details
 *
 * @brief Unit test for MeasureTime class
 *
 * @author s3mat3
 */

#include <thread>
#include "measure_time.hpp"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

//TEST_CASE("expire", "sec")
TEST_CASE("construct1")
{
    Sml::MeasureTime m(true);
    // m.start();
    CHECK(m(1000) == false);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    CHECK(m(1000) == true);

}

TEST_CASE("construct2")
{
    Sml::MeasureTime m(false);
    // m.start();
    CHECK(m(1000) == false);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    CHECK(m(1000) == false);
}

TEST_CASE("expire1")
{
    Sml::MeasureTime m(true);
    // m.start();
    CHECK(m.isExpireSec(1, true) == false);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    CHECK(m.isExpireSec(1, true) == true);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    CHECK(m.isExpireSec(1, true) == true);
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    CHECK(m.isExpireSec(1, true) == false);

}
