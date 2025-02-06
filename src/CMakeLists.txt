add_executable(HumanGL
        main.cpp
        Expected.h
        WindowContext.cpp
        WindowContext.h
        StringUnorderedMap.h
        Camera.cpp
        Camera.h

        Golem.microgltf.h
        Village.microgltf.h

        Window/Window.cpp
        Window/Window.h
        Window/Controls.cpp
        Window/Controls.h

        OpenGL/VertexBuffer.h
        OpenGL/VertexBuffer.cpp
        OpenGL/VertexArray.cpp
        OpenGL/VertexArray.h
        OpenGL/IndicesBuffer.cpp
        OpenGL/IndicesBuffer.h
        OpenGL/Shader.cpp
        OpenGL/Shader.h
        OpenGL/ShaderProgramInstance.cpp
        OpenGL/ShaderProgramInstance.h
        OpenGL/ShaderProgram.cpp
        OpenGL/ShaderProgram.h
        OpenGL/Debug.cpp
        OpenGL/Debug.h
        OpenGL/Pipeline.cpp
        OpenGL/Pipeline.h

        Components/CameraController.cpp
        Components/CameraController.h
        Components/UserInterface.cpp
        Components/UserInterface.h
        Components/MeshRenderer.cpp
        Components/MeshRenderer.h
        Components/Animator.cpp
        Components/Animator.h

        MicroGLTF/Struct.cpp
        MicroGLTF/Struct.h

        Engine/Engine.cpp
        Engine/Engine.h
        Engine/AnimationSampler.cpp
        Engine/AnimationSampler.h
        Engine/Animation.cpp
        Engine/Animation.h
        Engine/Mesh.cpp
        Engine/Mesh.h
        Engine/EngineComponent.h
        Engine/Object.cpp
        Engine/Object.h
        Engine/FrameInfo.h
        Engine/Transform.cpp
        Engine/Transform.h

        Utility/StridedIterator.h

        FtGLM/func.cpp
        FtGLM/ft_glm.h
        FtGLM/vec2.h
        FtGLM/vec3.h
        FtGLM/vec4.h
        FtGLM/mat3.h
        FtGLM/mat4.h
        FtGLM/quat.h
        FtGLM/func.h
        Utility/EnumHelpers.h
)

target_compile_definitions(HumanGL PRIVATE GLFW_INCLUDE_NONE GLM_ENABLE_EXPERIMENTAL STB_IMAGE_IMPLEMENTATION)

target_link_libraries(HumanGL PRIVATE OpenGL::GL)
target_link_libraries(HumanGL PRIVATE glfw)
target_link_libraries(HumanGL PRIVATE glad)
target_link_libraries(HumanGL PRIVATE stb)
target_link_libraries(HumanGL PRIVATE imgui)

target_include_directories(HumanGL PUBLIC
        ${PROJECT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)
