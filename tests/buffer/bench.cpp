/**
 * @file bench.cpp
 *
 * @copylight © 2025 s3mat3
 * This code is licensed under the MIT License, see the LICENSE.txt file for details
 *
 * @brief bench mark for std::string, std::vector VS Sml::ByteBuffer
 *
 * @warning using google benchmark
 *
 * @author s3mat3
 */
#include <string>
#include <vector>
#include "benchmark/benchmark.h"

#include "byte_buffer.hpp"

using namespace Sml;
/**  .
 *
 * 1048576 rooms (1M)
 */
const size_type TEST_ROOMS = request_volume(rooms::V1K) * request_volume(rooms::V1K);
static void BM_string_creation_size(benchmark::State& state) {
  for (auto _ : state)
      std::string empty_string(TEST_ROOMS, 0x00);
}

static void BM_vector_creation_size(benchmark::State& state) {
  for (auto _ : state)
      std::vector<char> empty_vector(TEST_ROOMS, 0x00);
}

static void BM_buffer_creation_size(benchmark::State& state) {
  for (auto _ : state)
      ByteBuffer empty_buffer(TEST_ROOMS, 0xff);
}

BENCHMARK(BM_string_creation_size);
BENCHMARK(BM_vector_creation_size);
BENCHMARK(BM_buffer_creation_size);

static void BM_string_copy(benchmark::State& state) {
    std::string str(TEST_ROOMS, 'c');
    for (auto _ : state) {
        //std::string empty_string(TEST_ROOMS, 0x00);
        auto empty_string = str;
    }
}
static void BM_buffer_copy(benchmark::State& state) {
    ByteBuffer buffer(TEST_ROOMS, 'c');
    for (auto _ : state) {
        ByteBuffer empty_buffer(buffer);
    }
}
BENCHMARK(BM_string_copy);
BENCHMARK(BM_buffer_copy);

static void BM_string_move(benchmark::State& state) {
    for (auto _ : state) {
        std::string target_string(std::string(TEST_ROOMS, 'c'));
    }
}
static void BM_buffer_move(benchmark::State& state) {
    for (auto _ : state) {
        ByteBuffer target_buffer(ByteBuffer(TEST_ROOMS, 'c'));
    }
}
BENCHMARK(BM_string_move);
BENCHMARK(BM_buffer_move);

static void BM_string_append(benchmark::State& state) {
    std::string str(TEST_ROOMS, 'c');
    std::string empty_string;
    for (auto _ : state) {
        empty_string.append(str);
        empty_string.clear();
    }
}
static void BM_buffer_append(benchmark::State& state) {
    ByteBuffer buffer(TEST_ROOMS, 'c');
    ByteBuffer empty_buffer(TEST_ROOMS);
    for (auto _ : state) {
        // empty_buffer.append(buffer.const_ptr(), buffer.capacity());
        empty_buffer.append(buffer);
        empty_buffer.clear();
    }
}
static void BM_buffer_string_append(benchmark::State& state) {
    std::string str(TEST_ROOMS, 'c');
    ByteBuffer empty_buffer(TEST_ROOMS);
    for (auto _ : state) {
        // empty_buffer.append(buffer.const_ptr(), buffer.capacity());
        empty_buffer.append(str.data(), str.size());
        empty_buffer.clear();
    }
}
BENCHMARK(BM_string_append);
BENCHMARK(BM_buffer_append);
BENCHMARK(BM_buffer_string_append);

static void BM_string_assign(benchmark::State& state) {
    std::string str(TEST_ROOMS, 'c');
    std::string empty_string(TEST_ROOMS, 0x00);
    for (auto _ : state) {
        empty_string.assign(str);
    }
}
static void BM_buffer_assign(benchmark::State& state) {
    ByteBuffer buffer(TEST_ROOMS, 'c');
    ByteBuffer empty_buffer(TEST_ROOMS);
    for (auto _ : state) {
        empty_buffer.assign(buffer);
    }
}
static void BM_buffer_string_assign(benchmark::State& state) {
    std::string str(TEST_ROOMS, 'c');
    ByteBuffer empty_buffer(TEST_ROOMS);
    for (auto _ : state) {
        empty_buffer.assign(str.data(), str.size());
    }
}
BENCHMARK(BM_string_assign);
BENCHMARK(BM_buffer_assign);
BENCHMARK(BM_buffer_string_assign);

BENCHMARK_MAIN();
