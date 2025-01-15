#include <iostream>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "HumanGLConfig.h"
#include "Engine.h"
#include "Window.h"
#include "WindowContext.h"

constexpr GLuint WIDTH = 800, HEIGHT = 600;

static auto printStartInfo() -> void
{
    std::cout << "HumanGL " << HumanGL_VERSION_MAJOR << "." << HumanGL_VERSION_MINOR << std::endl;
    int version = gladLoadGL(glfwGetProcAddress);
    std::cout << "OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
}

auto start() -> Expected<void, std::string>
{
    auto e_window_context = WindowContext::Create(4, 1);
    if (!e_window_context)
        return Unexpected("Failed to create window context: " + std::move(e_window_context).error());

    auto e_window = Window::Create(WIDTH, HEIGHT, "HumanGL");
    if (!e_window)
        return Unexpected("Failed to create window: " + std::move(e_window).error());

    e_window->setAsCurrentContext();
    printStartInfo();

    auto engine = Engine(*std::move(e_window));
    engine.getWindow().setKeyCallback([](const Window& window, const int key, const int action, int mode) -> void
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            window.setShouldClose();
    });

    engine.run([](Engine& engine)
    {
        glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
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
