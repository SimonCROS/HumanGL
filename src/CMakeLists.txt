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
        OpenGL/IndicesBuffer.cpp
        OpenGL/IndicesBuffer.h
        OpenGL/Shader.cpp
        OpenGL/Shader.h
        OpenGL/ShaderProgram.cpp
        OpenGL/ShaderProgram.h
        OpenGL/ShaderProgramVariants.cpp
        OpenGL/ShaderProgramVariants.h
        OpenGL/Debug.cpp
        OpenGL/Debug.h
        StringUnorderedMap.h
        Camera.cpp
        Camera.h
        Controls.cpp
        Controls.h
        Scripts/CameraController.cpp
        Scripts/CameraController.h
        Scripts/Transform.cpp
        Scripts/Transform.h
        MicroGLTF/Struct.cpp
        MicroGLTF/Struct.h
        Engine/AnimationSampler.cpp
        Engine/AnimationSampler.h
        Engine/Animation.cpp
        Engine/Animation.h
        Golem.microgltf.h
        UserInterface.cpp
        UserInterface.h

        FtGLM/func.cpp
        FtGLM/func.h
        FtGLM/ft_glm.h
        FtGLM/vec2.h
        FtGLM/vec3.h
        FtGLM/vec4.h
        FtGLM/mat4.h
        FtGLM/quat.h
        FtGLM/mat3.h
)

target_compile_definitions(HumanGL PRIVATE GLFW_INCLUDE_NONE STB_IMAGE_IMPLEMENTATION)

target_link_libraries(HumanGL PRIVATE OpenGL::GL)
target_link_libraries(HumanGL PRIVATE glfw)
target_link_libraries(HumanGL PRIVATE glad)
target_link_libraries(HumanGL PRIVATE stb)
target_link_libraries(HumanGL PRIVATE imgui)

target_include_directories(HumanGL PUBLIC
        ${PROJECT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)
