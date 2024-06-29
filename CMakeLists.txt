cmake_minimum_required(VERSION 3.22)

# set the project name and version
project(HumanGL VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# FindOpenGL prefers GLVND by default when available
cmake_policy(SET CMP0072 NEW)

configure_file(HumanGLConfig.h.in HumanGLConfig.h)

find_package(X11)
if(APPLE AND X11_FOUND)
  set(CMAKE_FIND_FRAMEWORK NEVER)
  find_package(OpenGL REQUIRED)
  unset(CMAKE_FIND_FRAMEWORK)
else()
  find_package(OpenGL REQUIRED)
endif()
find_package(glfw3 3.3 REQUIRED)

add_subdirectory(lib/glad)

add_executable(HumanGL src/main.cpp)

target_link_libraries(HumanGL PRIVATE glfw)
target_link_libraries(HumanGL PRIVATE OpenGL::GL)
target_link_libraries(HumanGL PRIVATE glad)

target_include_directories(HumanGL PUBLIC ${PROJECT_BINARY_DIR})
