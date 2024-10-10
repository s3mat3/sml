#
# usage cmake -D CMAKE_BUILD_TYPE=(Debug | Release | '') -DCMAKE_EXPORT_COMPILE_COMMANDS=on
#
cmake_minimum_required (VERSION 3.24)
project(result-test-build)
set(TARGET_BASE "notification")
set(TARGET "${TARGET_BASE}")
set(TARGET_MISC_TEST "${TARGET}-misc")

set(TEST_TARGET_SOURCES_BASE ${SML_TEST_BASE}/${TARGET_BASE})

set(EXECUTABLE_OUTPUT_PATH ${SML_TEST_OUT_DIR}/${TARGET_BASE})
#############
# MISC_TEST #
#############
set(MISC_TEST_TARGET_SOURCES
  ${TEST_TARGET_SOURCES_BASE}/misc_test.cpp
  )
add_executable(${TARGET_MISC_TEST}  ${MISC_TEST_TARGET_SOURCES})
#
# include files
target_include_directories(${TARGET_MISC_TEST}
  PRIVATE ${TEST_SOURCES_BASE}
  PRIVATE ${TOOLS_TESTER_BASE}
  PUBLIC  ${SML_INCLUDE_BASE}
  )
target_compile_options(${TARGET_MISC_TEST}
  PRIVATE -O2 -g3 -finline-functions
  PRIVATE -Wall -Wextra -W -Wctor-dtor-privacy -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wreorder
  PRIVATE -DSML_DEBUG -DSML_ASSERT_CHECK -DSML_ASSERT_OK -DSML_TRACE
  )
target_compile_features(${TARGET_MISC_TEST} PRIVATE cxx_std_20)
