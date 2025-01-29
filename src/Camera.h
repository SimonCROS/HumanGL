//
// Created by loumarti on 1/15/25.
//

#ifndef CAMERA_H
#define CAMERA_H

#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Components/Transform.h"

enum ViewMode
{
    COLOR,
    TEXTURE
};

class Camera
{
private:
    ViewMode m_mode;
    glm::mat4 m_projectionMatrix{};

    Transform m_transform{};

public:
    explicit Camera(uint32_t width, uint32_t height, float fov);

    [[nodiscard]] auto getViewMode() const -> ViewMode { return m_mode; }
    [[nodiscard]] auto projectionMatrix() const -> const glm::mat4& { return m_projectionMatrix; }
    [[nodiscard]] auto transform() -> Transform& { return m_transform; }
    [[nodiscard]] auto transform() const -> const Transform& { return m_transform; }

    [[nodiscard]] glm::mat4 computeViewMatrix() const;
};


#endif //CAMERA_H
