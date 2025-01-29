//
// Created by Simon Cros on 1/17/25.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "FtGLM/ft_glm.h"

class Transform
{
private:
    ft_glm::vec3 m_position{};
    ft_glm::quat m_rotation = ft_glm::quat::identity();
    ft_glm::vec3 m_scale{1.0f};

public:
    [[nodiscard]] auto position() -> ft_glm::vec3& { return m_position; }
    [[nodiscard]] auto position() const -> const ft_glm::vec3& { return m_position; }
    [[nodiscard]] auto rotation() -> ft_glm::quat& { return m_rotation; }
    [[nodiscard]] auto rotation() const -> const ft_glm::quat& { return m_rotation; }
    [[nodiscard]] auto scale() -> ft_glm::vec3& { return m_scale; }
    [[nodiscard]] auto scale() const -> const ft_glm::vec3& { return m_scale; }
};

#endif //TRANSFORM_H
