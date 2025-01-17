//
// Created by Simon Cros on 1/17/25.
//

#ifndef MESH_H
#define MESH_H
#include "Scripts/Transform.h"
#include "glm/glm.hpp"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"

class Mesh
{
private:
    Transform m_transform;
    glm::vec3 m_pivot{};

public:
    [[nodiscard]] auto transform() -> Transform& { return m_transform; }
    [[nodiscard]] auto transform() const -> const Transform& { return m_transform; }

    virtual auto render(VertexArray& vao, Shader& shader) -> void = 0;

    [[nodiscard]] auto computeTransformMatrix() const -> glm::mat4;

    virtual ~Mesh() = default;
};

#endif //MESH_H
