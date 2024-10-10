/**
 * \file measure.cpp
 *
 * \copyright © 2024 s3mat3 
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Unit test for MeasureTime class
 *
 * \author s3mat3
 */

#include <thread>
#include "debug.hpp"
#include "measure_time.hpp"


int main()
{
    Sml::MeasureTime m(true);
    // m.start();
    auto lc = 0;
    while (! m.isExpire(500)) {
        SML_INFO("Hello busyloop 500ms: " + std::to_string(++lc));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    MARK();
    lc = 0;
    while (! m.isExpireSec(1)) {
        SML_INFO("Hello busyloop 1s: " + std::to_string(++lc));
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
