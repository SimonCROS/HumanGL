#include <iostream>
#include <vector>
#include <print>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Engine.h"
#include "Window.h"

constexpr GLuint WIDTH = 800, HEIGHT = 600;

int main()
{
    auto e_window = Window::Create(4, 1, WIDTH, HEIGHT, "HumanGL");
    if (!e_window)
    {
        std::cout << "Error: " << e_window.error() << std::endl;
        return EXIT_FAILURE;
    }


    Engine engine = Engine(*std::move(e_window)); // TODO change

    engine.getWindow().setCurrentContext();
    engine.getWindow().setKeyCallback([](const Window& window, const int key, const int action, int mode) -> void
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            window.setShouldClose();
    });

    const int version = gladLoadGL(glfwGetProcAddress);
    // std::cout << "HumanGL " << HumanGL_VERSION_MAJOR << "." << HumanGL_VERSION_MINOR << std::endl;
    std::cout << "OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    while (!glfwWindowShouldClose(engine.getGLFWwindow()))
    {
        glfwPollEvents();

        glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(engine.getGLFWwindow());
    }

    glfwTerminate();


    return 0;
}
