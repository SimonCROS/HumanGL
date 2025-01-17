//
// Created by loumarti on 1/15/25.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "Window.h"
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Scripts/Transform.h"

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
    glm::mat4 m_viewMatrix{};

    Transform m_transform{};

    // glm::vec3 m_position{};
    // // glm::vec3 m_direction{};
    // float m_pitch{};
    // float m_yaw{};
    // float m_roll{};

public:
    // Model
    glm::vec3 modelCenter = glm::vec3(0.0f, 0.0f, 0.0f); // absolute center of model

    explicit Camera(uint32_t width, uint32_t height, float fov);

    [[nodiscard]] auto getViewMode() const -> ViewMode { return m_mode; }
    [[nodiscard]] auto getViewMatrix() const -> glm::mat4 { return m_viewMatrix; }
    [[nodiscard]] auto getProjectionMatrix() const -> glm::mat4 { return m_projectionMatrix; }
    // [[nodiscard]] auto getPosition() const -> glm::vec3 { return m_position; }
    // [[nodiscard]] auto getDirection() const -> glm::vec3 { return m_direction; }

    // auto setPosition(const glm::vec3 position) -> void { m_position = position; }
    // auto setDirection(const glm::vec3 direction) -> void { m_direction = direction; }

    [[nodiscard]] glm::mat4 computeViewMatrix() const;
    [[nodiscard]] glm::mat4 computeMVP() const;

    [[nodiscard]] auto transform() -> Transform&
    {
        return m_transform;
    }

    [[nodiscard]] auto transform() const -> const Transform&
    {
        return m_transform;
    }
};


#endif //CAMERA_H
