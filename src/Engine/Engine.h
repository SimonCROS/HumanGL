//
// Created by Simon Cros on 1/13/25.
//

#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <chrono>
#include <unordered_set>

#include "Camera.h"
#include "EngineComponent.h"
#include "glad/gl.h"
#include "OpenGL/ShaderProgramVariants.h"
#include "Window/Window.h"

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
private:
    Window m_window;

    ClockType m_clock{};
    TimePoint m_start{};

    FrameInfo m_currentFrameInfo{};

    Camera m_camera;
    std::unordered_set<std::shared_ptr<EngineComponent>> m_components;

    std::optional<ShaderProgramVariants> m_defaultShaderProgramVariants;

    bool m_doubleSided{false};

public:
    static auto Create(Window&& window) -> Engine;

    Engine(Window&& window, Camera&& camera) noexcept;

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

    [[nodiscard]] auto camera() noexcept -> Camera&
    {
        return m_camera;
    }

    [[nodiscard]] auto camera() const noexcept -> const Camera&
    {
        return m_camera;
    }

    [[nodiscard]] auto controls() const noexcept -> Controls
    {
        return m_window.getCurrentControls();
    }

    auto run() -> void;

    template<class T, class... Args>
        requires std::derived_from<T, EngineComponent> && std::constructible_from<T, Args...>
    auto create(Args&&... args) -> void
    {
        m_components.emplace(std::make_shared<T>(std::forward<Args>(args)...));
    }

    template<class T, class... Args>
        requires std::derived_from<T, EngineComponent> && std::constructible_from<T, Args...>
    auto createAndGet(Args&&... args) -> std::shared_ptr<T>
    {
        return std::dynamic_pointer_cast<T>(*m_components.emplace(std::make_shared<T>(std::forward<Args>(args)...)).first);
    }

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

    auto makeShaderVariants(const std::string_view& str, const std::string& vertPath, const std::string& fragPath) -> Expected<ShaderProgramVariants, std::string>
    {
        auto e_shaderVariants = ShaderProgramVariants::Create(vertPath, fragPath);
        if (!e_shaderVariants)
            return Unexpected(std::move(e_shaderVariants).error());

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
