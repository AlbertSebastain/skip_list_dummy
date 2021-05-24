cmake_minimum_required(VERSION 2.8)
project(skiplist)
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
set(CMAKE_CXX_FLAGS "-pthread")
include_directories("${PROJECT_BINARY_DIR}")
if(${SERVER_MODE} STREQUAL "true")
    message("enable server mode")
    configure_file (
    "${PROJECT_SOURCE_DIR}/config.h.in"
    "${PROJECT_BINARY_DIR}/config.h"
    )
    aux_source_directory(. DIR_SRC)
    add_executable(skiplist_server ${DIR_SRC})
else()
configure_file (
  "${PROJECT_SOURCE_DIR}/config.h.in"
  "${PROJECT_BINARY_DIR}/config.h"
  )
  add_executable(skiplist ./performance_test/performance_test.cpp skiplist.h)
endif()
