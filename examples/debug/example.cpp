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

#include <string>
#include <iostream>

#include "debug.hpp"

class Hoge {
    int m_point = 100;
public:
    Hoge() {}
    ~Hoge() {}
    auto point() const noexcept {return m_point;}
    void point(int p) noexcept {m_point = p;}
};

std::string fuga="Hello world";
using namespace Sml;
static constexpr int N = 100;

SML_TEST_BEGIN(fromClass) {
    Hoge h;
    SML_ASSERT(h.point() == 100, "Initial value", true);
    h.point(0);
    SML_ASSERT(h.point() == 0, "Assign to 0 in point", true);
}
SML_TEST_END


int main()
{
    Hoge h;
    std::cout << "Logging fatal to debug" << std::endl;
    SML_FATAL("hogehoge");
    SML_ERROR("hogehoge");
    SML_WARN("hogehoge");
    SML_NOTICE("hogehoge");
    SML_INFO("hogehoge");
    SML_LOG("hogehoge");
    std::cout << "marker MARK" << std::endl;
    MARK();

    SML_LOG("VAR_DUMP int");
    std::cout << VAR_DUMP(N) << std::endl;
    SML_LOG("VAR_DUMP class");
    std::cout << VAR_DUMP(&h) << std::endl;
    SML_LOG("VAR_DUMP string");
    std::cout << VAR_DUMP(fuga) << std::endl;

    SML_LOG("ASSERT");
    SML_ASSERT(true, "Hello success", true);
    #undef SML_ASSERT_OK
    SML_ASSERT(true, "Hello success", true);
    #define SML_ASSERT_OK
    try {
        SML_ASSERT(false, "Hello success", false);
    } catch(Sml::Debug::sml_assert &e) {
        SML_FATAL(e.what());
        TRACE("in catch");
    }
    fromClass();
    return 0;
}

