cmake_minimum_required(VERSION 3.24)

if (NOT (DEFINED DOC_BASE_NAME))
  set(DOC_BASE_NAME "sml-doc")
endif()

project(${DOC_BASE_NAME}_project
  LANGUAGES NONE)

## if compile to smli document 
set(PROJ_DOC_SMLI 1) 

## if use subdirectory under correct

if (NOT (DEFINED SML_BASE_DIR))
  set(SML_BASE_DIR     ${PROJECT_SOURCE_DIR})
  set(SML_BUILD_DIR    ${PROJECT_BINARY_DIR})
  set(SML_DOC_OUT_DIR  ${SML_BUILD_DIR}/doc)
  set(SML_3RD_BASE     ${SML_BASE_DIR}/3rd)
  set(SML_INCLUDE_BASE ${SML_BASE_DIR}/include)
  set(SML_DOC_BASE     ${SML_BASE_DIR}/doc-base)
endif()

if(NOT EXISTS ${SML_DOC_OUT_DIR})
  file(MAKE_DIRECTORY ${SML_DOC_OUT_DIR})
endif()

find_package(Doxygen)
if(DOXYGEN_FOUND)
        set(DOXYGEN_CONF_FILE "Doxyfile")
        set(DOXYGEN_CONF_PATH   ${SML_DOC_BASE}/${DOXYGEN_CONF_FILE})
        set(DOXYGEN_TARGET ${DOC_BASE_NAME})
        CONFIGURE_FILE(${DOXYGEN_CONF_PATH}.in ${DOXYGEN_CONF_PATH})
        ADD_CUSTOM_TARGET(${DOXYGEN_TARGET}
                        ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONF_PATH}
			            WORKING_DIRECTORY ${SML_DOC_OUT_DIR})
else(DOXYGEN_FOUND)
        message (WARNING "doxygen binary couldn't be found")
endif(DOXYGEN_FOUND)
