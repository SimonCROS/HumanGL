//
// Created by Simon Cros on 1/17/25.
//

#include "MeshComponent.h"

auto MeshComponent::render(VertexArray& vao, Shader& shader) -> void
{
    if (m_mesh == nullptr)
        return;

    m_mesh->m_vertexBuffer.bind();
    m_mesh->m_indicesBuffer.bind();
    vao.addBuffer(m_mesh->m_vertexBuffer, 0, 3);
    vao.addBuffer(m_mesh->m_colorsBuffer, 1, 3);
    shader.setUniformMat4f("transform", computeTransformMatrix());
    glDrawElements(GL_TRIANGLES, m_mesh->indicesCount(), GL_UNSIGNED_INT, nullptr);
}

auto MeshComponent::computeTransformMatrix() const -> glm::mat4
{
    constexpr auto identity = glm::identity<glm::mat4>();
    const auto rotate = glm::translate(identity, m_pivot * -1.0f)
        * glm::mat4_cast(m_transform.rotation())
        * glm::translate(identity, m_pivot);

    return rotate
        * glm::scale(identity, m_transform.scale())
        * glm::translate(identity, m_transform.position() - (m_pivot * m_transform.scale()));
}
