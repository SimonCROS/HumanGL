//
// Created by Simon Cros on 1/13/25.
//

#include "Window.h"

auto Window::Create(const int glMajor, const int glMinor, const int width, const int height, const std::string& title) -> Expected<Window, std::string>
{
    if (glfwInit() == GLFW_FALSE)
    {
        const char* description;
        glfwGetError(&description);

        return Unexpected(std::string("Error while creating the window context: ") + description);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        const char* description;
        glfwGetError(&description);

        if (glfwGetError(&description) != GLFW_NO_ERROR) {
            return Unexpected("Error while creating the window: " + std::string(description));
        }
        return Unexpected("Error while creating the window: Unknown error.");
    }

    return Expected<Window, std::string>(std::in_place, window);
}
