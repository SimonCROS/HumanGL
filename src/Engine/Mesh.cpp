//
// Created by Simon Cros on 1/17/25.
//

#include "Mesh.h"

auto Mesh::computeTransformMatrix() const -> glm::mat4
{
    constexpr auto identity = glm::identity<glm::mat4>();
    const auto rotate = glm::translate(identity, m_pivot * -1.0f)
        * glm::mat4_cast(m_transform.rotation())
        * glm::translate(identity, m_pivot);

    return rotate
        * glm::scale(identity, m_transform.scale())
        * glm::translate(identity, m_transform.position() - (m_pivot * m_transform.scale()));
}
