//
// Created by Simon Cros on 1/13/25.
//

#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <chrono>
#include "Window.h"
#include "glad/gl.h"

using ClockType = std::chrono::steady_clock;
using DurationType = std::chrono::duration<float>;
using TimePoint = std::chrono::time_point<ClockType, DurationType>;

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
