//
// Created by Simon Cros on 1/13/25.
//

#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <chrono>
#include "Window.h"
#include "glad/gl.h"
#include "OpenGL/Debug.h"

using ClockType = std::chrono::steady_clock;
using TimePoint = ClockType::time_point;
using DurationType = std::chrono::duration<float>;

struct FrameInfo
{
    uint64_t frameCount;
    DurationType deltaTime;
};

class Engine
{
public:
    using LoopCallbackType = std::function<void(Engine&)>;

private:
    Window m_window;

    ClockType m_clock{};
    TimePoint m_start{};

    FrameInfo m_currentFrameInfo{};

public:
    explicit Engine(Window&& window) noexcept : m_window(std::move(window))
    {
        m_window.setAsCurrentContext();
        const int version = gladLoadGL(glfwGetProcAddress);
        std::cout << "OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

        const bool hasDebugOutput = GLAD_GL_KHR_debug || GLAD_GL_ARB_debug_output;

        int flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (hasDebugOutput && (flags & GL_CONTEXT_FLAG_DEBUG_BIT))
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(glDebugOutput, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
        }
    }

    [[nodiscard]] auto getWindow() noexcept -> Window&
    {
        return m_window;
    }

    [[nodiscard]] auto getWindow() const noexcept -> const Window&
    {
        return m_window;
    }

    [[nodiscard]] auto getFrameInfo() const noexcept -> FrameInfo
    {
        return m_currentFrameInfo;
    }

    auto run(const LoopCallbackType& callback) -> void;
};

#endif //ENGINE_H
