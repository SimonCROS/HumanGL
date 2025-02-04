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
    std::reference_wrapper<ShaderProgram>& m_program; // TODO Change

    auto bindTexture(Engine& engine, ShaderProgramInstance& program, int textureIndex,
                     const std::string_view& bindingKey, GLint bindingValue) -> void; // TMP
    auto renderMesh(Engine& engine, int meshIndex, const ft_glm::mat4& transform) -> void;
    auto renderNode(Engine& engine, int nodeIndex, ft_glm::mat4 transform) -> void;

public:
    explicit MeshRenderer(Object& object, const Mesh& model, std::reference_wrapper<ShaderProgram>& program) :
        EngineComponent(object), m_mesh(model), m_program(program)
    {
    }

    [[nodiscard]] auto mesh() const -> const Mesh& { return m_mesh; }

    auto setAnimator(const Animator& animator) -> void { m_animator = animator; }
    auto unsetAnimator() -> void { m_animator = std::nullopt; }

    auto onRender(Engine& engine) -> void override;
};

#endif //MODELRENDERER_H
