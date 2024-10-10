#
# usage cmake -D CMAKE_BUILD_TYPE=(Debug | Release | '') -DCMAKE_EXPORT_COMPILE_COMMANDS=on
#
cmake_minimum_required (VERSION 3.24)
project(result-test-build)
set(TARGET_BASE "result")
set(TARGET "${TARGET_BASE}-test")
set(TARGET_COMPLEX "${TARGET_BASE}-complex")
set(RESULT_UNIT_TEST "${TARGET}-unit-test")

set(TEST_TARGET_SOURCES_BASE ${SML_TEST_BASE}/${TARGET_BASE})

set(TEST_TARGET_SOURCES
  ${TEST_TARGET_SOURCES_BASE}/unit_test.cpp
  )
set(COMPLEX_TARGET_SOURCES
  ${TEST_TARGET_SOURCES_BASE}/complex_test.cpp
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
  )
#
# test define
add_test(
  NAME ${TARGET} # テスト名
  COMMAND ${TARGET} # mylib::hoge というテストのみ実行する
 # CONFIGURATIONS Release # テスト構成がReleaseのときのみ実行
  WORKING_DIRECTORY ${SML_TEST_OUT_DIR} # 実行ディレクトリは、ビルドディレクトリ直下のtmpディレクトリ
  )
#
#
# final executable target
# add_executable(${TARGET_COMPLEX}  ${COMPLEX_TARGET_SOURCES})
# #
# # include files
# target_include_directories(${TARGET_COMPLEX}
#   PRIVATE ${TEST_SOURCES_BASE}
#   PRIVATE ${TOOLS_TESTER_BASE}
#   PUBLIC  ${SML_INCLUDE_BASE}
#   )
# target_compile_options(${TARGET}
#   PRIVATE -O2 -finline-functions -std=c++20
#   PRIVATE -Wall -Wextra -W -Wctor-dtor-privacy -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wreorder
#   )
