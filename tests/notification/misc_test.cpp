/**
 * \file misc_test.cpp
 *
 * \copyright © 2024 s3mat3
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief
 *
 * \author s3mat3
 */

#include "notification.hpp"

using namespace Sml;

return_code function_void()
{
    return FAILURE;
}

return_code function_i(return_code i)
{
    return i;
}

class SomeClass
{
public:
    return_code function_i(return_code i) {return i;}
    return_code function_v() {return FAILURE;}
};

using void_notification = Notification<>;

SML_TEST_BEGIN(freeFunc) {
    Notification v_noti;
    v_noti.connect(function_void);
    SML_ASSERT((v_noti), "void member", true);
    SML_ASSERT(v_noti() == FAILURE, "-1", true);

    Notification<return_code> i_noti;//(function_i, SML_ARGC(1));
    i_noti.connect(function_i, std::placeholders::_1);
    SML_ASSERT((i_noti), "int member", true);
    SML_ASSERT(i_noti(1000) == 1000, "1000", true);

    Notification vv_noti(function_void);
    SML_ASSERT((vv_noti), "void member", true);
    SML_ASSERT(vv_noti() == FAILURE, "-1", true);

    auto value = FAILURE;
    Notification vvv_noti([&value]{value = OK; return value;});
    SML_ASSERT((vvv_noti), "void member", true);
    vvv_noti();
    SML_ASSERT(value == OK, "OK", true);

    Notification<return_code> ii_noti(function_i, SML_ARGC(1));
    SML_ASSERT((ii_noti), "int member", true);
    SML_ASSERT(ii_noti(10000) == 10000, "10000", true);

    Notification<return_code> iii_noti([&value](return_code a){value = a; return a;}, SML_ARGC(1));
    SML_ASSERT((iii_noti), "int member", true);
    iii_noti(-1000);
    SML_ASSERT(value == -1000, "lambda", true);
}
SML_TEST_END

SML_TEST_BEGIN(fromClass) {
    SomeClass s;
    void_notification v_noti(&SomeClass::function_v, &s);
    SML_ASSERT((v_noti), "void member direct construct", true);
    SML_ASSERT(v_noti() == FAILURE, "-1", true);

    Notification<return_code> i_noti(&SomeClass::function_i, &s, SML_ARGC(1));
    SML_ASSERT((i_noti), "int member direct construct", true);
    SML_ASSERT(i_noti(1000) == 1000, "1000", true);
}
SML_TEST_END

int main()
{
    freeFunc();
    fromClass();
    return OK;
}

