add_executable(HumanGL
        main.cpp
        Expected.h
        Engine.cpp
        Engine.h
        Window.cpp
        Window.h
        WindowContext.cpp
        WindowContext.h
        OpenGL/VertexBuffer.h
        OpenGL/VertexBuffer.cpp
        OpenGL/VertexArray.cpp
        OpenGL/VertexArray.h
        OpenGL/Shader.cpp
        OpenGL/Shader.h
        Camera.cpp
        Camera.h
        OpenGL/IndicesBuffer.cpp
        OpenGL/IndicesBuffer.h
)

target_compile_definitions(HumanGL PRIVATE GLFW_INCLUDE_NONE)

target_link_libraries(HumanGL PRIVATE OpenGL::GL)
target_link_libraries(HumanGL PRIVATE glfw)
target_link_libraries(HumanGL PRIVATE glad)
target_link_libraries(HumanGL PRIVATE glm::glm)

target_include_directories(HumanGL PUBLIC
        ${PROJECT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)
