//
// Created by Simon Cros on 1/29/25.
//

#ifndef MODELRENDERER_H
#define MODELRENDERER_H
#include "Engine/EngineComponent.h"
#include "Engine/Mesh.h"
#include "OpenGL/ShaderProgramVariants.h"

class MeshRenderer final : public EngineComponent
{
private:
    const Mesh& m_mesh;
    ShaderProgram m_program;

    auto bindTexture(Engine& engine, int textureIndex, const std::string_view& bindingKey, GLint bindingValue) -> void; // TMP
    auto renderMesh(Engine& engine, int meshIndex, const glm::mat4& transform) -> void;
    auto renderNode(Engine& engine, int nodeIndex, glm::mat4 transform) -> void;

public:
    explicit MeshRenderer(const Mesh& model, ShaderProgramVariants& programs) : m_mesh(model), m_program(programs.getProgram(ShaderFlags::ShaderHasNone))
    {
    }

    auto onRender(Engine& engine) -> void override;
};

#endif //MODELRENDERER_H
