//
// Created by Simon Cros on 1/13/25.
//

#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <functional>
#include <unordered_set>

#include "FrameInfo.h"
#include "glad/gl.h"
#include "MicroGLTF/Struct.h"
#include "OpenGL/ShaderProgram.h"
#include "Window/Window.h"

extern GLuint whiteTexture; // TMP

class Camera;
class Mesh;
class Object;

class Engine
{
public:
    using ModelRef = std::reference_wrapper<Mesh>;
    using ObjectRef = std::reference_wrapper<Object>;
    using ShaderProgramVariantsRef = std::reference_wrapper<ShaderProgram>;

    using ModelPtr = std::unique_ptr<Mesh>;
    using ObjectPtr = std::unique_ptr<Object>;
    using ShaderProgramPtr = std::unique_ptr<ShaderProgram>;

private:
    Window m_window;

    ClockType m_clock{};
    TimePoint m_start{};

    FrameInfo m_currentFrameInfo{};

    StringUnorderedMap<ModelPtr> m_models;
    StringUnorderedMap<ShaderProgramPtr> m_shaders;
    std::unordered_set<ObjectPtr> m_objects;

    bool m_doubleSided{false};
    GLenum m_polygonMode{GL_FILL};
    GLuint m_currentShaderProgram{0};

    const Camera* m_camera{nullptr};

public:
    static auto Create(Window&& window) -> Engine;

    explicit Engine(Window&& window) noexcept;

    [[nodiscard]] auto getWindow() noexcept -> Window& { return m_window; }
    [[nodiscard]] auto getWindow() const noexcept -> const Window& { return m_window; }

    [[nodiscard]] auto frameInfo() const noexcept -> FrameInfo { return m_currentFrameInfo; }

    [[nodiscard]] auto controls() const noexcept -> Controls { return m_window.getCurrentControls(); }

    [[nodiscard]] auto isDoubleSided() const noexcept -> bool { return m_doubleSided; }
    [[nodiscard]] auto polygonMode() const noexcept -> GLenum { return m_polygonMode; }

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

    auto setPolygoneMode(const GLenum polygonMode) -> void
    {
        if (m_polygonMode != polygonMode)
        {
            m_polygonMode = polygonMode;
            glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
        }
    }

    auto useProgram(const ShaderProgramInstance& program) -> void
    {
        if (m_currentShaderProgram != program.id())
        {
            m_currentShaderProgram = program.id();
            program.use();
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
        -> Object&;

    auto setCamera(const Camera& camera) -> void { m_camera = &camera; }
};

#endif //ENGINE_H
