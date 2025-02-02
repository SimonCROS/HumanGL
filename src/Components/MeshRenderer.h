//
// Created by Simon Cros on 1/29/25.
//

#ifndef MODELRENDERER_H
#define MODELRENDERER_H
#include "Animator.h"
#include "Engine/EngineComponent.h"
#include "Engine/Mesh.h"
#include "OpenGL/ShaderProgram.h"

class MeshRenderer final : public EngineComponent
{
private:
    const Mesh& m_mesh;
    std::optional<std::reference_wrapper<const Animator>> m_animator;
    ShaderProgramInstance m_program;

    auto bindTexture(Engine& engine, int textureIndex, const std::string_view& bindingKey,
                     GLint bindingValue) -> void; // TMP
    auto renderMesh(Engine& engine, int meshIndex, const glm::mat4& transform) -> void;
    auto renderNode(Engine& engine, int nodeIndex, glm::mat4 transform) -> void;

public:
    explicit MeshRenderer(Object& object, const Mesh& model, ShaderProgram& programs) :
        EngineComponent(object), m_mesh(model), m_program(programs.getProgram(ShaderFlags::ShaderHasNone))
    {
    }

    auto setAnimator(const Animator& animator) -> void { m_animator = animator; }
    auto unsetAnimator() -> void { m_animator = std::nullopt; }

    auto onRender(Engine& engine) -> void override;
};

#endif //MODELRENDERER_H
