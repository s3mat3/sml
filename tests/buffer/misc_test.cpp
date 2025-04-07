/**
 * @file misc_test.cpp
 *
 * @copyright © 2025 s3mat3 
 *
 * This code is licensed under the MIT License, see the LICENSE.txt file for details
 *
 * @brief misc check
 *
 * @author s3mat3
 */

# include "byte_buffer.hpp"

struct SomeClass {
    SomeClass() = default;
    SomeClass(int x, int y) : x{x}, y{y} {}
    int x = 1;
    int y = 2;
};
static constexpr Sml::size_type TEST_ROOMS = request_volume(Sml::rooms::V16K) * request_volume(Sml::rooms::V16K);

int main()
{
    SomeClass arr[10] = {{0,0},{1,-1},{2,-2},{3,-3},{4,-4},{5,-1},{6,-2},{7,-3},{8,-4},{9,-4}};
    Sml::ByteBuffer s(1024);
    Sml::ByteBuffer x(s);
    auto y = x;
    auto z = std::move(x);
    Sml::BufferBase<SomeClass> some_buffer(10);
    DUMP(s.capacity());
    DUMP(x.capacity());
    DUMP(y.capacity());
    // y.push_back('H');
    some_buffer.assign(&arr[0], some_buffer.capacity());
    DUMP(some_buffer.size());
    // check resize
    Sml::ByteBuffer t = {0,1,2,3,4,5,6,7,8,9};
    DUMP(t.capacity());
    t.push_back(10);
    DUMP(t.capacity());
    SML_ASSERT(t.capacity() == 20, "CPACITY check", true);

    Sml::ByteBuffer a(TEST_ROOMS, 0x00);
    SML_ASSERT(a.capacity() == TEST_ROOMS, "CPACITY check", true);
    a.push_back(0x03);
    SML_ASSERT(a.capacity() == TEST_ROOMS * 2, "CPACITY check", true);
    SML_ASSERT(a[TEST_ROOMS] == 0x03, "contents check", true);
    DUMP(TEST_ROOMS);
    DUMP(TEST_ROOMS * 2);
    DUMP(a.capacity());
    VAR_DUMP(a.capacity());
    DUMP(a.size());
    MARK();
    SML_LOG("Hoge" + VAR_DUMP(a.capacity()));
    SML_INFO("Hoge" + VAR_DUMP(a.capacity()));
    SML_WARN("Hoge"  + VAR_DUMP(a.capacity()));
    SML_NOTICE("Hoge" + VAR_DUMP(a.capacity()));
    SML_ERROR("Hoge" + VAR_DUMP(a.capacity()));
    SML_FATAL("Hoge" + VAR_DUMP(a.capacity()));
    return Sml::OK;
}
