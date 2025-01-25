# ---------------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------------
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "Enable C++20 features in GLM")

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INCLUDE_NONE OFF CACHE BOOL "" FORCE)

# ---------------------------------------------------------------------------------
# Find OpenGL
# ---------------------------------------------------------------------------------
# FindOpenGL prefers GLVND by default when available
cmake_policy(SET CMP0072 NEW)

find_package(X11)
if(APPLE AND X11_FOUND)
    set(CMAKE_FIND_FRAMEWORK NEVER)
    find_package(OpenGL 4.1 REQUIRED)
    unset(CMAKE_FIND_FRAMEWORK)
else()
    find_package(OpenGL 4.1 REQUIRED)
endif()

# ---------------------------------------------------------------------------------
# Download or retrieve glm and glfw
# ---------------------------------------------------------------------------------
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #refs/tags/1.0.1
        EXCLUDE_FROM_ALL
)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG dc46d3f8129712e42856c20e99a604a3b08ad581 #refs/tags/3.3.10
        FIND_PACKAGE_ARGS 3.3 NAMES glfw3
        EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(glm glfw)

# ---------------------------------------------------------------------------------
# Add libraries subdirectories
# ---------------------------------------------------------------------------------
add_subdirectory(${EXTERNAL_LIBRARIES_DIR}/glad EXCLUDE_FROM_ALL)
add_subdirectory(${EXTERNAL_LIBRARIES_DIR}/stb EXCLUDE_FROM_ALL)
add_subdirectory(${EXTERNAL_LIBRARIES_DIR}/imgui EXCLUDE_FROM_ALL)
