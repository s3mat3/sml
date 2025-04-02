#[[
 usage cmake
 -DCMAKE_BUILD_TYPE=(Debug | Release | Memcheck)
 -DCMAKE_EXPORT_COMPILE_COMMANDS=on
 -DCMAKE_VERBOSE_MAKEFILE:BOOL=TRUE
 -DSML_BUILD_TEST=ON (default ON) we will compile code
 -DSML_BUILD_EXAMPLES=ON (default ON) we will compile example code
 -DSML_BUILD_DOC=ON (default ON) we will generate source document
]]
cmake_minimum_required (VERSION 3.24)
file(READ VERSION SML_VERSION_STRING)
string(STRIP ${SML_VERSION_STRING} SML_VERSION_STRING)

project(
  sml
  VERSION ${SML_VERSION_STRING}
  LANGUAGES CXX
  )

option(SML_BUILD_TEST "Build test" OFF)
option(SML_BUILD_BENCHMARK "Build benchmark test (with SML_BUILD_TEST)" OFF)
option(SML_BUILD_EXAMPLES "Build examples" ON)
option(SML_BUILD_DOC "Build documents" OFF)

set(SML_DEVELOP_TOOLS_BASE "/opt/tools")
if (SML_BUILD_TEST)
  include(CTest)
  # include(FetchContent)
  # FetchContent_Declare(
  #   doctest
  #   GIT_REPOSITORY https://github.com/doctest/doctest.git
  #   GIT_TAG master
  #   )
  #   FetchContent_MakeAvailable(doctest)
  #   include_directories(${doctest_SOURCE_DIR}/doctest)
  #   set(TOOLS_TESTER_NAME "doctest")
  #   set(TOOLS_TESTER_BASE ${doctest_SOURCE_DIR}/doctest/${TOOLS_TESTER_NAME})
    set(TOOLS_TESTER_BASE ${SML_DEVELOP_TOOLS_BASE}/include/doctest)
  if (SML_BUILD_BENCHMARK)
    # FetchContent_Declare(
    #   benchmark
    #   GIT_REPOSITORY https://github.com/google/benchmark.git
    #   GIT_TAG main
    #   )
    # set(BENCHMARK_ENABLE_GTEST_TESTS OFF)
    # FetchContent_MakeAvailable(doctest benchmark)
    # include_directories(${doctest_SOURCE_DIR}/doctest
    #   ${benchmark_SOURCE_DIR}/include/benchmark)
    # set(TOOLS_BENCHMARK_NAME "benchmark")
    set(TOOLS_BENCHMARK_BASE "/opt/tools")
    set(TOOLS_BENCHMARK_INCLUDE ${TOOLS_BENCHMARK_BASE}/include)
    set(TOOLS_BENCHMARK_LIB     ${TOOLS_BENCHMARK_BASE}/lib)
  # else ()
  endif()
endif()

set(TARGET_BASE "sml-base")
set(SML_BASE_LIB ${TARGET_BASE})
if (NOT (DEFINED SML_VERSION))
  #
  # SET VERSION NUMBER
  #
  EXECUTE_PROCESS (COMMAND date +%F OUTPUT_VARIABLE BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)
  EXECUTE_PROCESS (COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE BUILD_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE)
  EXECUTE_PROCESS (COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE BUILD_DESTINATION OUTPUT_STRIP_TRAILING_WHITESPACE)
  EXECUTE_PROCESS (COMMAND cat ${PROJECT_SOURCE_DIR}/VERSION OUTPUT_VARIABLE BUILD_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)

  set (SML_BUILD_DATE ${BUILD_DATE})
  set (SML_VERSION "ver. ${BUILD_VERSION}")
  set (SML_DOC_VERSION ${BUILD_VERSION})
  set (SML_REVISION ${BUILD_REVISION})
  set (SML_VERSION_REVISION "${SML_BUILD_DATE} ${BUILD_VERSION}-${SML_REVISION}")
endif()
####################
## base directories
####################
set(SML_BASE_DIR     ${PROJECT_SOURCE_DIR})
set(SML_BUILD_DIR    ${PROJECT_BINARY_DIR})
set(SML_3RD_BASE     ${SML_BASE_DIR}/3rd)
set(SML_INCLUDE_BASE ${SML_BASE_DIR}/include)
set(SML_DOC_BASE     ${SML_BASE_DIR}/doc-base)
set(SML_DOC_OUT_DIR  ${SML_BUILD_DIR}/doc)
set(SML_TEST_BASE    ${SML_BASE_DIR}/tests)
set(SML_EXAMPLE_BASE ${SML_BASE_DIR}/examples)
## compile options
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT (DEFINED SML_LIB_OUT_DIR))
  set(SML_LIB_OUT_DIR ${SML_BUILD_DIR}/lib)
endif()

set(LIBRARY_OUTPUT_PATH ${SML_LIB_OUT_DIR})

if (SML_BUILD_TEST)
  enable_testing()
  if (NOT (DEFINED SML_TEST_BASE))
    set(SML_TEST_BASE ${SML_BASE_DIR}/tests)
  endif()
  if (NOT (DEFINED SML_IO_TEST_BASE))
    set(SML_IO_TEST_BASE ${SML_TEST_BASE}/io)
  endif()

  if(NOT (DEFINED SML_TEST_OUT_DIR))
    set(SML_TEST_OUT_DIR ${PROJECT_BINARY_DIR}/sml/tests)
  endif()
  if(NOT (DEFINED SML_IO_TEST_OUT_DIR))
    set(SML_IO_TEST_OUT_DIR ${SML_TEST_OUT_DIR}/io)
  endif()

  add_subdirectory(${SML_TEST_BASE}/measure_time)
  add_subdirectory(${SML_TEST_BASE}/result)
  add_subdirectory(${SML_TEST_BASE}/notification)
  # add_subdirectory(${SML_TEST_BASE}/storage)
  # add_subdirectory(${SML_TEST_BASE}/buffer)
  # add_subdirectory(${SML_IO_TEST_BASE}/serial)
endif()

if (SML_BUILD_EXAMPLES)
  find_program(TOOL_MEMORY_CHECKER "valgrind")
  if (TOOL_MEMORY_CHECKER)
    set(TOOL_THREAD_CHECKER ${TOOL_MEMORY_CHECKER})
    set(TOOL_MEMORY_CHECK_OPTIONS
      --tool=memcheck
      --leak-check=full
      --show-reachable=yes
      --track-fds=yes
      )
    set(TOOL_THREAD_CHECK_OPTIONS
      --tool=helgrind
      )
    message("We have a memory checker called Valgrind")
  endif()
  # check and examples directories
  if(NOT (DEFINED SML_EXAMPLE_BASE))
    set(SML_EXAMPLE_BASE ${SML_BASE_DIR}/examples)
  endif()
  if(NOT (DEFINED SML_IO_EXAMPLE_BASE))
    set(SML_IO_EXAMPLE_BASE ${SML_BASE_DIR}/examples/io)
  endif()
  # examples output directories
  if(NOT (DEFINED SML_EXAMPLE_OUT_DIR))
    set(SML_EXAMPLE_OUT_DIR ${PROJECT_BINARY_DIR}/sml/examples)
  endif()
  if(NOT (DEFINED SML_IO_EXAMPLE_OUT_DIR))
    set(SML_IO_EXAMPLE_OUT_DIR ${PROJECT_BINARY_DIR}/sml/examples/io)
  endif()
  add_subdirectory(${SML_EXAMPLE_BASE}/debug)
  add_subdirectory(${SML_EXAMPLE_BASE}/measure_time)
  add_subdirectory(${SML_EXAMPLE_BASE}/fsm)
  add_subdirectory(${SML_EXAMPLE_BASE}/result)
  add_subdirectory(${SML_EXAMPLE_BASE}/notification)
  add_subdirectory(${SML_EXAMPLE_BASE}/signal)
  add_subdirectory(${SML_EXAMPLE_BASE}/thread)
  add_subdirectory(${SML_IO_EXAMPLE_BASE}/serial)
  #add_subdirectory(${SML_IO_EXAMPLE_BASE}/device)
  # add_subdirectory(${SML_EXAMPLE_BASE}/singleton)
endif()
##############
### documents
##############
if (SML_BUILD_DOC)
  add_subdirectory(${SML_DOC_BASE})
endif()

