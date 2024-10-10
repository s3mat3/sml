/**
 * \file example.cpp
 *
 * \copyright © 2024 s3mat3 
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief Example for notification
 *
 * \author s3mat3
 */

#include <iostream>

#include "debug.hpp"
#include "notification.hpp"
#include "thread.hpp"


using notification_int_t = Sml::Notification<int>;
using notification_string_t = Sml::Notification<std::string&>;

class SomeCaller
{
public:
    SomeCaller() : run(true) {}
    void stop() {run = false;}

    void function1(Sml::void_ptr)
    {
        size_t l = 0;
        while(run) {
            std::string hello(__FUNCTION__);
            hello.append(std::to_string(l++));
            SML_LOG("notify " +  hello);
            aaa.notify(l);
            bbb.notify(hello);
            Sml::Thread::sleep(100);
            if (l > 20) break;
        }
    }
public:
    notification_int_t    aaa;
    notification_string_t bbb;
private:
    bool run;
};

class SomeCallee
{
public:
    SomeCallee() : run(true) {}
    void stop() {run = false;}

    void function1(Sml::void_ptr)
    {
        size_t l = 0;
        while(run) {
            std::string hello(__FUNCTION__);
            SML_LOG("Hello " +  hello + std::to_string(l++));
            Sml::Thread::sleep(100);
            if (l > 20) break;
        }
    }

    int function2(int a)
    {
        std::string x = std::to_string(a) + " get from Caller";
        SML_LOG(x);
        return a;
    }

    int function3(std::string a)
    {
        SML_LOG(a);
        return 0;
    }
public:
private:
    bool run;
};

int main()
{
    using runnable_t = Sml::RunnableAdapter<SomeCaller>;
    // setup caller Thread
    auto caller = std::make_shared<SomeCaller>();
    auto runner1 = std::make_shared<runnable_t>(caller, &SomeCaller::function1);
    Sml::Thread th1(runner1, "TH1");
    // setup callee object
    auto callee  = std::make_shared<SomeCallee>();
    // connect callback on 2way
    caller->aaa.connect(&SomeCallee::function2, callee, SML_ARGC(1));
    Sml::connect(caller->bbb, &SomeCallee::function3, callee, SML_ARGC(1));
    th1.start(nullptr);
    return 0;
}
