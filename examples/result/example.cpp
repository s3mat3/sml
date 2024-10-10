/*! \file complex_test.cpp
 *
 * \copyright © 2024 s3mat3
 *
 * This code is licensed under the MIT License, see the LICENSE file for details
 *
 * \brief example for result class
 *
 * \author s3mat3
 */
#include <vector>
#include "sml.hpp"
#include "debug.hpp"
#include "result.hpp"

using ByteBuffer = std::string;

using namespace Sml;
class Some
{
public:
    using byte_buffer = ByteBuffer;
    using result_t = Result<byte_buffer>;
    Some(int i, double d) : a(i), b(d) {}
    Some() : Some(0, 0.0) {}
    auto func1() -> result_t
    {
        auto x = std::to_string(a) + " is int value " + std::to_string(b);
        return x;
    }
    auto func2(int a) -> result_t
    {
        if (a > 5)
            return func1();
        else
            return result_t(error_type(-100));
    }
    void aa(int i) { a = i;}
    void bb(double d) { b = d;}
    double bb() {return b;}
private:
    int    a;
    double b;
};



int main()
{
    Some some;
    auto x = some.func1();
    DUMP(x.value());
    some.aa(3);
    some.bb(3.0);
    auto y = some.func2(2);
    if (!(y))
        DUMP(y.error());
    return 0;
}

