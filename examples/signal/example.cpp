/**
 * \file example.cpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief
 *
 * \author s3mat3
 */

#include "debug.hpp"
#include "signal.hpp"
#include "thread.hpp"

using namespace Sml;
static constexpr int N = 100;
static constexpr int M = 10000;
int main()
{
    Sml::Signal s;
    std::thread th1([&]{ // writer thread
        try {
            for (int i = 1; i <= N; ++i) {
                s.update(i + M);
                Sml::Thread::sleep(1);
            }
        } catch (canceled_wait_event&) {
            SML_LOG("Cancele catch IN th1");
        }
    });
    std::thread th2([&]{ // reader thread
        try {
            while (1) {
                auto x = s.wait_update();
                SML_LOG(VAR_DUMP(x));
            }
        } catch (canceled_wait_event&) {
            SML_LOG("Cancele catch IN th2");
        }
    });
    th1.join();
    s.cancel();
    th2.join();
    return 0;
}

