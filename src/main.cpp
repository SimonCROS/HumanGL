#include <iostream>
// #include <vector>
// #include <print>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "Engine.h"
#include "Window.h"

constexpr GLuint WIDTH = 800, HEIGHT = 600;

/*std::vector<int> fun()
{
    return {5};
}*/

void key_callback(GLFWwindow *window, const int key, int scancode, const int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

int main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    auto e_window = Window::Create(WIDTH, HEIGHT, "HumanGL");
    if (!e_window)
    {
        // std::print("Error: {}", e_window.error());
        std::cout << "Failed to create GLFW window : " << e_window.error() << std::endl;
        return EXIT_FAILURE;
    }


    Engine engine = Engine(*std::move(e_window)); // TODO change


    glfwMakeContextCurrent(engine.getGLFWwindow());

    glfwSetKeyCallback(engine.getGLFWwindow(), key_callback);

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
