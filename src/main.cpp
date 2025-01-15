#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "HumanGLConfig.h"
#include "Engine.h"
#include "Window.h"
#include "WindowContext.h"
#include "openGL/Shader.h"
#include "openGL/VertexArray.h"
#include "openGL/VertexBuffer.h"

constexpr GLuint WIDTH = 800, HEIGHT = 600;

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

    auto vertexArray = VertexArray();
    vertexArray.bind();

    // An array of 3 vectors which represents 3 vertices
    static const GLfloat g_vertex_buffer_data[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         0.0f,  1.0f, 0.0f,
      };

    VertexBuffer vertexBuffer = VertexBuffer(g_vertex_buffer_data, 3 * 3 * sizeof(GLfloat));
    vertexBuffer.bind();

    Shader shader = Shader("./res/shaders/basic_shader.glsl");
    shader.bind();

    shader.unbind();
    vertexArray.unbind();
    vertexBuffer.unbind();

    engine.run([&](Engine& engine)
    {
        glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // ---------

        shader.bind();
        vertexArray.bind();
        vertexBuffer.bind();

        vertexArray.addBuffer(vertexBuffer, 0, 3);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glDisableVertexAttribArray(0);
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
