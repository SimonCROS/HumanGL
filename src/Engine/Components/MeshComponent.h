//
// Created by Simon Cros on 1/17/25.
//

#ifndef MESH_H
#define MESH_H
#include "Component.h"
#include "Engine/Mesh.h"
#include "Engine/Transform.h"
#include "glm/glm.hpp"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"

class Mesh;

class MeshComponent : public Component
{
private:
    Transform m_transform{};
    glm::vec3 m_pivot{};

    Mesh* m_mesh{nullptr};

public:
    MeshComponent() = default;
    explicit MeshComponent(Mesh* mesh) : m_mesh(mesh) {  }

    [[nodiscard]] auto transform() -> Transform& { return m_transform; }
    [[nodiscard]] auto transform() const -> const Transform& { return m_transform; }
    [[nodiscard]] auto pivot() -> glm::vec3& { return m_pivot; }
    [[nodiscard]] auto pivot() const -> const glm::vec3& { return m_pivot; }

    auto setMesh(Mesh* mesh) -> void { m_mesh = mesh; }

    virtual auto render(VertexArray& vao, Shader& shader) -> void;

    [[nodiscard]] auto computeTransformMatrix() const -> glm::mat4;
};

#endif //MESH_H
