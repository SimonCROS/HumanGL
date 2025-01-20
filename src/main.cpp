#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "HumanGLConfig.h"
#include "Camera.h"
#include "Engine.h"
#include "Window.h"
#include "WindowContext.h"
#include "Engine/Cube.h"
#include "OpenGL/IndicesBuffer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/ShaderProgramVariants.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "Scripts/CameraController.h"

auto start() -> Expected<void, std::string>
{
    std::cout << "HumanGL " << HumanGL_VERSION_MAJOR << "." << HumanGL_VERSION_MINOR << std::endl;

    auto e_window_context = WindowContext::Create(4, 1);
    if (!e_window_context)
        return Unexpected("Failed to create window context: " + std::move(e_window_context).error());

    auto e_window = Window::Create(WIDTH, HEIGHT, "HumanGL");
    if (!e_window)
        return Unexpected("Failed to create window: " + std::move(e_window).error());

    auto engine = Engine(*std::move(e_window));
    engine.getWindow().setKeyCallback([](const Window& window, const int key, const int action, int mode) -> void
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            window.setShouldClose();
    });

    auto camera = Camera(engine.getWindow().width(), engine.getWindow().height(), 60.0f);

    auto vertexArray = VertexArray();
    vertexArray.bind();

    Cube c1;
    c1.transform().scale() = {1.8f, 2.5f, 1.0f};

    Cube c2;
    c2.transform().scale() = {0.8f, 1.5f, 0.8f};
    c2.transform().position() = {-4.0f, 0.0f, 0.0f};

    Cube c2_child;
    c2_child.transform().scale() = {0.8f, 1.5f, 0.8f};
    c2_child.transform().position() = {-4.0f, -2.5f, 0.0f};

    Cube c3;
    c3.transform().scale() = {0.8f, 1.5f, 0.8f};
    c3.transform().position() = {4.0f, 0.0f, 0.0f};

    Cube c3_child;
    c3_child.transform().scale() = {0.8f, 1.5f, 0.8f};
    c3_child.transform().position() = {4.0f, -2.5f, 0.0f};


    c2.addChild(&c2_child);
    c3.addChild(&c3_child);
    c1.addChild(&c2);
    c1.addChild(&c3);

    ShaderProgramVariants programVariants(RESOURCE_PATH "shaders/default.vert", RESOURCE_PATH "shaders/default.frag");
    programVariants.enableVariant(ShaderHasNone);

    glfwSetInputMode(engine.getWindow().getGLFWHandle(), GLFW_STICKY_KEYS, GL_TRUE);
    /* gl display Settings */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Transform tr{};
    CameraController c(tr, 15);

    engine.run([&](Engine& engine)
    {
        glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Will be automatized
        c.update(engine.getWindow().getCurrentControls(), camera, engine.getFrameInfo().deltaTime.count());

        const auto pvMat = camera.projectionMatrix() * camera.computeViewMatrix();
        for (auto& [flags, program] : programVariants.programs)
        {
            program.use();
            program.setMat4("projectionView", pvMat);
        }

        vertexArray.bind();
        c1.renderFamily(vertexArray, programVariants.getProgram(ShaderHasNone));

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
    });

    return {};
}

auto main() -> int
{
    auto e_result = start();
    if (!e_result)
    {
        std::cerr << "Error: " << e_result.error() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
