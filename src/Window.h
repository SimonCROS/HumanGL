//
// Created by Simon Cros on 1/13/25.
//

#ifndef WINDOW_H
#define WINDOW_H
#include <string>
#include <functional>
#include <utility>
#include <GLFW/glfw3.h>

#include "Expected.h"

class Window
{
public:
    using KeyListener = std::function<void (Window&, int, int, int)>;

private:
    GLFWwindow* m_window;
    KeyListener m_keyListener;

    auto glfwKeyListeneraaa(GLFWwindow* m_window, int key, int scancode, int action, int mode) -> void
    {
        if (m_keyListener)
        {
            std::invoke(m_keyListener, *this, key, action, mode);
        }
    }

    static auto glfwKeyListener(GLFWwindow* m_window, int key, int scancode, int action, int mode) -> void
    {
    }

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

    auto setCurrentContext() const -> void
    {
        glfwMakeContextCurrent(m_window);
    }

    auto setKeyCallback(const KeyListener& callback) -> void
    {
        m_keyListener = callback;
    }

    auto setShouldClose() const -> void
    {
        glfwSetWindowShouldClose(m_window, GL_TRUE);
    }
};

#endif //WINDOW_H
