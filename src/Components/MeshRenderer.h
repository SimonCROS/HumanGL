//
// Created by Simon Cros on 1/29/25.
//

#ifndef MODELRENDERER_H
#define MODELRENDERER_H
#include "Engine/EngineComponent.h"
#include "Engine/Mesh.h"

class MeshRenderer final : public EngineComponent
{
private:
    const Mesh& m_mesh;

    auto bindTexture(int textureIndex, const std::string_view& bindingKey, GLint bindingValue) -> void; // TMP
    auto renderMesh(int meshIndex, const glm::mat4& transform) -> void;
    auto renderNode(int nodeIndex, glm::mat4 transform) -> void;

public:
    explicit MeshRenderer(const Mesh& model) : m_mesh(model)
    {
    }

    auto onRender(Engine& engine) -> void override;
};

#endif //MODELRENDERER_H
