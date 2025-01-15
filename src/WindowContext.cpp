//
// Created by Simon Cros on 1/15/25.
//

#include "WindowContext.h"

auto WindowContext::Create(const int glMajor, const int glMinor) -> Expected<WindowContext, std::string>
{
    if (glfwInit() == GLFW_FALSE)
    {
        const char* description;
        glfwGetError(&description);

        return Unexpected(std::string("Failed to initialize glfw: ") + description);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    return WindowContext();
}
