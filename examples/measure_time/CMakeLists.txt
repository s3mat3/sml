#
# usage cmake -D CMAKE_BUILD_TYPE=(Debug | Release | '') -DCMAKE_EXPORT_COMPILE_COMMANDS=on
#
cmake_minimum_required (VERSION 3.24)
project(debug-example-build)
set(TARGET_BASE "measure_time")
set(TARGET "${TARGET_BASE}-example")

set(EXAMPLE_SOURCES_BASE ${SML_EXAMPLE_BASE}/${TARGET_BASE})

set(EXAMPLE_TARGET_SOURCES
  ${EXAMPLE_SOURCES_BASE}/example.cpp
  )
message(${EXAMPLE_SOURCES_BASE})
set(EXECUTABLE_OUTPUT_PATH ${SML_EXAMPLE_OUT_DIR}/${TARGET_BASE})
#
# final executable target
add_executable(${TARGET}  ${EXAMPLE_TARGET_SOURCES})
#
# target_link_libraries(${TARGET}
#  PRIVATE pthread
#  )
#
# include files
target_include_directories(${TARGET}
  PRIVATE ${EXAMPLE_SOURCES_BASE}
  PRIVATE  ${SML_INCLUDE_BASE}
  )
target_compile_options(${TARGET}
  PRIVATE -O2 -g3 -finline-functions #-fpic -pedantic
  PRIVATE -Wall -Wextra -W -Wctor-dtor-privacy -Wnon-virtual-dtor -Wold-style-cast -Woverloaded-virtual -Wreorder
  PRIVATE -DSML_DEBUG -DSML_TRACE -DSML_ASSERT_CHECK -DSML_ASSERT_OK
  )
target_compile_features(${TARGET} PRIVATE cxx_std_20)
