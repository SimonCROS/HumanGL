//
// Created by Simon Cros on 1/13/25.
//

#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include "Window.h"
#include "glad/gl.h"

class Engine
{
public:
    using LoopCallbackType = std::function<void(Engine&)>;

private:
    Window m_window;

public:
    explicit Engine(Window&& window) noexcept : m_window(std::move(window))
    {
        m_window.setAsCurrentContext();
        // m_window.asCurrentContext([]()
        // {
        const int version = gladLoadGL(glfwGetProcAddress);
        std::cout << "OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
        // });
    }

    [[nodiscard]] auto getWindow() noexcept -> Window&
    {
        return m_window;
    }

    [[nodiscard]] auto getWindow() const noexcept -> const Window&
    {
        return m_window;
    }

    auto run(const LoopCallbackType& callback) -> void;
};

#endif //ENGINE_H
