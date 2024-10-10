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

#include "signal_tower.hpp"
using namespace Sml;
int main()
{
    SignalTower st;
    SignalFSM fsm(&st);
    fsm.onAnyEvent();
    return 0;
}

