#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "HumanGLConfig.h"
#include "Camera.h"
#include "Engine.h"
#include "Window.h"
#include "WindowContext.h"
#include "Engine/Cuboid.h"
#include "OpenGL/IndicesBuffer.h"
#include "OpenGL/Shader.h"
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

    // [2] Set Cubes object instance
    Cuboid cube1 = Cuboid();
    cube1.translate(glm::vec3(0.0f, 4.0f, 0.0f));
    Cuboid cube2 = Cuboid();
    cube2.scaleVertexBuffer(glm::vec3(1.0f, 2.5f, 1.8f));
    cube2.translate(glm::vec3(0.0f, 0.0f, 0.0f)); // no move yet

    VertexBuffer cube1VertexBuffer = VertexBuffer(cube1.getVertexBuffer(), 24 * sizeof(GLfloat));
    cube1VertexBuffer.bind();

    VertexBuffer cube2VertexBuffer = VertexBuffer(cube2.getVertexBuffer(), 24 * sizeof(GLfloat));
    cube2VertexBuffer.bind();

    // [3] Set Indices buffer (bind to OpenGL in init)
    IndicesBuffer indicesBuffer = IndicesBuffer(Cuboid::s_indices_buffer, sizeof(Cuboid::s_indices_buffer));

    VertexBuffer colorBuffer = VertexBuffer(Cuboid::s_color_buffer, sizeof(Cuboid::s_color_buffer));
    colorBuffer.bind();

    Shader shader = Shader("./res/shaders/current_shader.glsl");
    shader.bind();

    shader.unbind();
    vertexArray.unbind();
    cube1VertexBuffer.unbind();
    cube2VertexBuffer.unbind();
    colorBuffer.unbind();

    glfwSetInputMode(engine.getWindow().getGLFWHandle(), GLFW_STICKY_KEYS, GL_TRUE);
    /* gl display Settings */
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    Transform tr{};
    tr.position().y = 2;
    CameraController c(tr, 15);

    engine.run([&](Engine& engine)
    {
        glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Will be automatized
        c.update(engine.getWindow().getCurrentControls(), camera, engine.getFrameInfo().deltaTime.count());

        // ---------

        shader.bind();
        shader.setUniformMat4f("u_mvp", camera.computeMVP());

        vertexArray.addBuffer(colorBuffer, 1, 3);

        vertexArray.addBuffer(cube2VertexBuffer, 0, 3);
        shader.setUniformMat4f("u_mvp", camera.computeMVP() * cube2.getModel());
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        vertexArray.addBuffer(cube1VertexBuffer, 0, 3);
        shader.setUniformMat4f("u_mvp", camera.computeMVP() * cube1.getModel());
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // ----------
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
