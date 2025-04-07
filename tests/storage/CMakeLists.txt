#
# usage cmake -D CMAKE_BUILD_TYPE=(Debug | Release | '') -DCMAKE_EXPORT_COMPILE_COMMANDS=on
#
cmake_minimum_required (VERSION 3.24)
project(storage-test-build)
set(TARGET_BASE "storage")
set(TARGET "${TARGET_BASE}-test")
set(RESULT_UNIT_TEST "${TARGET}-unit-test")
set(TARGET_BENCHMARK "${TARGET_BASE}-benchmark")

set(TEST_TARGET_SOURCES_BASE ${SML_TEST_BASE}/${TARGET_BASE})

set(TEST_TARGET_SOURCES
  ${TEST_TARGET_SOURCES_BASE}/unit_test.cpp
  )
set(BENCHMARK_TARGET_SOURCES
  ${TEST_TARGET_SOURCES_BASE}/bench.cpp
  )

set(EXECUTABLE_OUTPUT_PATH ${SML_TEST_OUT_DIR}/${TARGET_BASE})
#
# final executable target
add_executable(${TARGET}  ${TEST_TARGET_SOURCES})
#
# include files
target_include_directories(${TARGET}
  PRIVATE ${TEST_SOURCES_BASE}
  PRIVATE ${TOOLS_TESTER_BASE}
  PRIVATE  ${SML_INCLUDE_BASE}
  )
target_compile_options(${TARGET}
  PRIVATE -O2 -g3 -finline-functions -std=c++20
  PRIVATE -Wall -Wextra -W -Wctor-dtor-privacy -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wreorder
  PRIVATE -DSML_DEBUG -DSML_ASSERT_CHECK -DSML_ASSERT_OK -DSML_TRACE
  )
#
# test define
add_test(
  NAME ${TARGET}
  COMMAND ${TARGET}
 # CONFIGURATIONS Release
  WORKING_DIRECTORY ${SML_TEST_OUT_DIR}
  )
##
# benchmark
#
add_executable(${TARGET_BENCHMARK}  ${BENCHMARK_TARGET_SOURCES})
target_link_directories(${TARGET_BENCHMARK}
  PRIVATE ${SML_LIB_OUT_DIR}
### When use local host tools, under uncomment
  PRIVATE ${TOOLS_BENCHMARK_LIB}
### When use fetch content, under uncomment
#  PRIVATE ${benchmark_SOURCE_DIR}
  )
#
# link libraries
target_link_libraries(${TARGET_BENCHMARK}
  PRIVATE "pthread"
  PRIVATE "benchmark"
  )
#
# include files
target_include_directories(${TARGET_BENCHMARK}
  PRIVATE ${TEST_SOURCES_BASE}
  PRIVATE ${TOOLS_TESTER_BASE}
  PRIVATE ${SML_INCLUDE_BASE}
  PRIVATE ${SML_INTERNAL}
### When use local host tools, under uncomment
  PRIVATE ${TOOLS_BENCHMARK_INCLUDE}
### When use fetch content, under uncomment
#  PRIVATE ${benchmark_SOURCE_DIR}/include/benchmark
  )
target_compile_options(${TARGET_BENCHMARK}
  PRIVATE -O2 -mtune=native -march=native -finline-functions -flto -std=c++20
  PRIVATE -Wall -Wextra -W -Wctor-dtor-privacy -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wreorder
  )
#
# test define
add_test(
  NAME ${TARGET_BENCHMARK}
  COMMAND ${TARGET_BENCHMARK}
 # CONFIGURATIONS Release
  WORKING_DIRECTORY ${SML_TEST_OUT_DIR}
  )
