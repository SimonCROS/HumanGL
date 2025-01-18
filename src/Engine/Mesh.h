//
// Created by Simon Cros on 1/17/25.
//

#ifndef MESH_H
#define MESH_H

#include "Scripts/Transform.h"
#include "glm/glm.hpp"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"
#include <vector>
#include <algorithm>

class Mesh
{
private:
    Transform m_transform;
    glm::vec3 m_pivot{};
    /* --- */
    Mesh* m_parent = nullptr;
    std::vector<Mesh*> m_children;

public:
    [[nodiscard]] auto transform() -> Transform& { return m_transform; }
    [[nodiscard]] auto transform() const -> const Transform& { return m_transform; }

    virtual auto render(VertexArray& vao, Shader& shader) -> void = 0;
    virtual auto renderFamily(VertexArray& vao, Shader& shader) -> void = 0;


    [[nodiscard]] auto computeTransformMatrix() const -> glm::mat4;

    virtual ~Mesh() = default;

    /* --- */
    [[nodiscard]] auto parent() const -> Mesh* { return m_parent; }
    [[nodiscard]] auto children() const -> const std::vector<Mesh*>& { return m_children; }
    auto setParent(Mesh* parent) -> void;
    auto addChild(Mesh* child) -> void;
    auto removeChild(Mesh* child) -> void;
    [[nodiscard]] auto hasChild(const Mesh* child) const -> bool;

private:
    auto appendChild(Mesh* child) -> bool;
    auto eraseChild(Mesh* child) -> bool;

    auto findChild(const Mesh* child) -> std::vector<Mesh*>::iterator;
    [[nodiscard]] auto findChild(const Mesh* child) const -> std::vector<Mesh*>::const_iterator;




};

#endif //MESH_H
