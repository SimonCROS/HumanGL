//
// Created by Simon Cros on 1/13/25.
//

#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <chrono>
#include <functional>
#include <unordered_set>

#include "Camera.h"
#include "Mesh.h"
#include "Object.h"
#include "glad/gl.h"
#include "MicroGLTF/Struct.h"
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
public:
    using ModelRef = std::reference_wrapper<Mesh>;
    using ObjectRef = std::reference_wrapper<Object>;
    using ShaderProgramVariantsRef = std::reference_wrapper<ShaderProgramVariants>;

    using ModelPtr = std::unique_ptr<Mesh>;
    using ObjectPtr = std::unique_ptr<Object>;
    using ShaderProgramVariantsPtr = std::unique_ptr<ShaderProgramVariants>;

private:
    Window m_window;

    ClockType m_clock{};
    TimePoint m_start{};

    FrameInfo m_currentFrameInfo{};

    Camera m_camera;

    StringUnorderedMap<ModelPtr> m_models;
    StringUnorderedMap<ShaderProgramVariantsPtr> m_shaders;
    std::unordered_set<ObjectPtr> m_objects;

    bool m_doubleSided{false};

public:
    static auto Create(Window&& window) -> Engine;

    Engine(Window&& window, Camera&& camera) noexcept;

    [[nodiscard]] auto getWindow() noexcept -> Window& { return m_window; }
    [[nodiscard]] auto getWindow() const noexcept -> const Window& { return m_window; }

    [[nodiscard]] auto frameInfo() const noexcept -> FrameInfo { return m_currentFrameInfo; }

    [[nodiscard]] auto camera() noexcept -> Camera& { return m_camera; }
    [[nodiscard]] auto camera() const noexcept -> const Camera& { return m_camera; }

    [[nodiscard]] auto controls() const noexcept -> Controls { return m_window.getCurrentControls(); }

    auto run() -> void;

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

    [[nodiscard]]
    auto
    makeShaderVariants(const std::string_view& id, const std::string& vertPath, const std::string& fragPath)
        -> Expected<ShaderProgramVariantsRef, std::string>;

    [[nodiscard]]
    auto
    loadModel(const std::string_view& id, const microgltf::Model& gltfModel)
        -> Expected<ModelRef, std::string>;

    [[nodiscard]]
    auto
    instantiate()
        -> ObjectRef;

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
