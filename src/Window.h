//
// Created by Simon Cros on 1/13/25.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <string>
// #include <format>
#include <utility>
#include <GLFW/glfw3.h>

#include "Expected.h"

class Window
{
private:
    GLFWwindow* m_window;

public:
    explicit Window(GLFWwindow* window) noexcept : m_window(window)
    {
    }

    Window(const Window&) = delete;

    Window(Window&& other) noexcept : m_window(std::exchange(other.m_window, nullptr))
    {
    }

    ~Window()
    {
        if (m_window != nullptr)
        {
            glfwDestroyWindow(m_window);
        }
    }

    auto operator=(const Window&) -> Window& = delete;

    auto operator=(Window&& other) noexcept -> Window&
    {
        std::swap(m_window, other.m_window);
        return *this;
    }

    [[nodiscard]] static auto Create(int width, int height, const std::string& title) -> Expected<Window, std::string>;

    [[nodiscard]] GLFWwindow* getGLFWwindow() noexcept
    {
        return m_window;
    }
};

#endif //WINDOW_H
