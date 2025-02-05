cmake_minimum_required(VERSION 3.22)
include(FetchContent)

project(HumanGL VERSION 1.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(HumanGLConfig.h.in HumanGLConfig.h)

set(EXTERNAL_LIBRARIES_DIR ${CMAKE_SOURCE_DIR}/lib)
include(${CMAKE_SOURCE_DIR}/cmake/SetupExternalLibraries.cmake)

add_subdirectory(src)
add_subdirectory(tests)
