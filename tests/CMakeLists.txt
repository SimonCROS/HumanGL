# ---------------------------------------------------------------------------------
# Download or retrieve glm
# ---------------------------------------------------------------------------------
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "Enable C++20 features in GLM")

FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0af55ccecd98d4e5a8d1fad7de25ba429d60e863 #refs/tags/1.0.1
        EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(glm)

add_executable(ftglm-tests
        ../src/FtGLM/func.cpp
        ../src/FtGLM/ft_glm.h
        ../src/FtGLM/vec2.h
        ../src/FtGLM/vec3.h
        ../src/FtGLM/vec4.h
        ../src/FtGLM/mat3.h
        ../src/FtGLM/mat4.h
        ../src/FtGLM/quat.h
        ../src/FtGLM/func.h

        ft_glm_tests.cpp
        ft_glm_tests.h
)

target_compile_definitions(ftglm-tests PRIVATE GLM_ENABLE_EXPERIMENTAL)
target_link_libraries(ftglm-tests PRIVATE glm::glm)
target_include_directories(ftglm-tests PRIVATE ../src)
