//
// Created by Simon Cros on 1/13/25.
//

#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <chrono>
#include "Window.h"
#include "glad/gl.h"

extern GLuint whiteTexture; // TMP

using ClockType = std::chrono::steady_clock;
using TimePoint = ClockType::time_point;
using DurationType = std::chrono::duration<float>;

struct FrameInfo
{
    uint64_t frameCount;
    DurationType time;
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

    bool m_doubleSided{false};

public:
    explicit Engine(Window&& window) noexcept;

    [[nodiscard]] auto getWindow() noexcept -> Window&
    {
        return m_window;
    }

    [[nodiscard]] auto getWindow() const noexcept -> const Window&
    {
        return m_window;
    }

    [[nodiscard]] auto frameInfo() const noexcept -> FrameInfo
    {
        return m_currentFrameInfo;
    }

    auto run(const LoopCallbackType& callback) -> void;

    auto setDoubleSided(const bool value) -> void
    {
        if (m_doubleSided != value)
        {
            m_doubleSided = value;
            if (value)
                glDisable(GL_CULL_FACE);
            else
                glEnable(GL_CULL_FACE);
        }
    }

    static auto useLineDisplayMode() -> void
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    static auto useFillDisplayMode() -> void
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
};

#endif //ENGINE_H
