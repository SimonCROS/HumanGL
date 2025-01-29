add_executable(HumanGL
        main.cpp
        Expected.h
        WindowContext.cpp
        WindowContext.h
        StringUnorderedMap.h
        Camera.cpp
        Camera.h

        Golem.microgltf.h

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
        OpenGL/ShaderProgram.cpp
        OpenGL/ShaderProgram.h
        OpenGL/ShaderProgramVariants.cpp
        OpenGL/ShaderProgramVariants.h
        OpenGL/Debug.cpp
        OpenGL/Debug.h

        Components/CameraController.cpp
        Components/CameraController.h
        Components/Transform.cpp
        Components/Transform.h
        Components/UserInterface.cpp
        Components/UserInterface.h

        MicroGLTF/Struct.cpp
        MicroGLTF/Struct.h

        Engine/Engine.cpp
        Engine/AnimationSampler.cpp
        Engine/AnimationSampler.h
        Engine/Animation.cpp
        Engine/Animation.h
        Engine/Mesh.cpp
        Engine/Mesh.h
        Engine/EngineComponent.h
        Engine/Object.cpp
        Engine/Object.h
        Components/MeshRenderer.cpp
        Components/MeshRenderer.h
)

target_compile_definitions(HumanGL PRIVATE GLFW_INCLUDE_NONE GLM_ENABLE_EXPERIMENTAL STB_IMAGE_IMPLEMENTATION)

target_link_libraries(HumanGL PRIVATE OpenGL::GL)
target_link_libraries(HumanGL PRIVATE glfw)
target_link_libraries(HumanGL PRIVATE glad)
target_link_libraries(HumanGL PRIVATE glm::glm)
target_link_libraries(HumanGL PRIVATE stb)
target_link_libraries(HumanGL PRIVATE imgui)

target_include_directories(HumanGL PUBLIC
        ${PROJECT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
)
