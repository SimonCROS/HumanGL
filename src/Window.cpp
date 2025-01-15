//
// Created by Simon Cros on 1/13/25.
//

#include "Window.h"

auto Window::Create(const int width, const int height, const std::string& title) -> Expected<Window, std::string>
{
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    // GLFWwindow* window = nullptr; // [+] TODO delete after use (only test)

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
