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

    static auto glfwKeyListener(GLFWwindow* glfwWindow, int key, int scancode, int action, int mode) -> void
    {
        auto* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
        if (window != nullptr && window->m_keyListener != nullptr)
        {
            std::invoke(window->m_keyListener, *window, key, action, mode);
        }
    }

public:
    [[nodiscard]] static auto Create(int width, int height, const std::string& title) -> Expected<Window, std::string>;

    explicit Window(GLFWwindow* glfwWindow) noexcept : m_window(glfwWindow)
    {
        glfwSetWindowUserPointer(glfwWindow, this);
        glfwSetKeyCallback(glfwWindow, &Window::glfwKeyListener);
    }

    Window(const Window&) = delete;

    Window(Window&& other) noexcept
        : m_window(std::exchange(other.m_window, nullptr)),
          m_keyListener(std::exchange(other.m_keyListener, nullptr))
    {
        glfwSetWindowUserPointer(m_window, this);
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
        std::swap(m_keyListener, other.m_keyListener);
        glfwSetWindowUserPointer(m_window, this);
        return *this;
    }

    auto setAsCurrentContext() const -> void
    {
        glfwMakeContextCurrent(m_window);
    }

    auto asCurrentContext(const std::function<void()>& func) const -> void
    {
        const auto prev = glfwGetCurrentContext();
        if (prev == m_window)
        {
            std::invoke(func);
        }
        else
        {
            glfwMakeContextCurrent(m_window);
            std::invoke(func);
            glfwMakeContextCurrent(prev);
        }
    }

    auto setKeyCallback(const KeyListener& callback) -> void
    {
        m_keyListener = callback;
    }

    auto setShouldClose() const -> void
    {
        glfwSetWindowShouldClose(m_window, GLFW_TRUE);
    }

    [[nodiscard]] auto shouldClose() const -> bool
    {
        return glfwWindowShouldClose(m_window);
    }

    auto update() const -> bool // NOLINT(*-use-nodiscard)
    {
        if (shouldClose())
            return false;
        glfwPollEvents();
        return true;
    }

    auto swapBuffers() const -> void
    {
        glfwSwapBuffers(m_window);
    }

    // [!] temporary
    [[nodiscard]] auto getGLFWHandle() const -> GLFWwindow*
    {
        return m_window;
    }
};

#endif //WINDOW_H
