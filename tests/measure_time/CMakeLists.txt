#
# usage cmake -D CMAKE_BUILD_TYPE=(Debug | Release | '') -DCMAKE_EXPORT_COMPILE_COMMANDS=on
#
cmake_minimum_required (VERSION 3.24)
project(masure-time-test-build)
set(TARGET_BASE "measure_time")
set(TARGET "${TARGET_BASE}-test")
set(MEASURE_TIME_UNIT_TEST "measure-time-unit-test")

set(TEST_TARGET_SOURCES_BASE ${SML_TEST_BASE}/${TARGET_BASE})

set(TEST_TARGET_SOURCES
  ${TEST_TARGET_SOURCES_BASE}/unit_test.cpp
  )

set(EXECUTABLE_OUTPUT_PATH ${SML_TEST_OUT_DIR}/${TARGET_BASE})
#
# final executable target
add_executable(${TARGET}  ${TEST_TARGET_SOURCES})
#
# link libraries このセクションは必ずadd_executableマクロの後ろに記述する必要有り
target_link_libraries(${TARGET}
  PRIVATE "-pthread"
  )
#
# include files
target_include_directories(${TARGET}
  PRIVATE ${TEST_SOURCES_BASE}
  PRIVATE ${TOOLS_TESTER_BASE}
  PUBLIC  ${SML_INCLUDE_BASE}
  )
#
# test define
add_test(
  NAME ${TARGET} # テスト名は hoge_test
  COMMAND ${TARGET} # mylib::hoge というテストのみ実行する
 # CONFIGURATIONS Release # テスト構成がReleaseのときのみ実行
  WORKING_DIRECTORY ${SML_TEST_OUT_DIR} # 実行ディレクトリは、ビルドディレクトリ直下のtmpディレクトリ
  )
