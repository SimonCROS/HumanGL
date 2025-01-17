//
// Created by Simon Cros on 1/17/25.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

class Transform
{
private:
    glm::vec3 m_position{};
    glm::quat m_rotation = glm::identity<glm::quat>();

public:
    [[nodiscard]] auto position() -> glm::vec3&
    {
        return m_position;
    }

    [[nodiscard]] auto position() const -> const glm::vec3&
    {
        return m_position;
    }

    [[nodiscard]] auto rotation() -> glm::quat&
    {
        return m_rotation;
    }

    [[nodiscard]] auto rotation() const -> const glm::quat&
    {
        return m_rotation;
    }
};

#endif //TRANSFORM_H
