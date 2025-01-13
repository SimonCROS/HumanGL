//
// Created by Simon Cros on 1/13/25.
//

#include "Window.h"

auto Window::Create(const int width, const int height, const std::string& title) -> Expected<Window, std::string>
{
    GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if (window == nullptr)
    {
        const char* description;
        glfwGetError(&description);

        return Unexpected(std::format("Error while creating the window: {}", description));
    }

    return Expected<Window, std::string>(std::in_place, window);
}
