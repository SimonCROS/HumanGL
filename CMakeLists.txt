cmake_minimum_required(VERSION 3.22)
include(FetchContent)

project(HumanGL VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

configure_file(HumanGLConfig.h.in HumanGLConfig.h)

include(.cmake/OpenGL.cmake)
add_subdirectory(lib)
add_subdirectory(src)
