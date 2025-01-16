//
// Created by Simon Cros on 1/16/25.
//

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "Camera.h"
#include "Controls.h"
#include "glm/glm.hpp"
#include <glm/gtx/norm.hpp>

class CameraController {
private:
    glm::mat4 m_modelMatrix;
    glm::mat4 m_translationMatrix;
    glm::mat4 m_rotationMatrix;

    // Model Rotation
    bool m_autoRotate = false;
    float m_lastRotateSwitchTime = 0.0f;
    float m_lastWireframeSwitchTime = 0.0f;
    float m_rotationRate = 2.0f;

    const float m_speed = 2.0f;

public:
    auto update(const Controls& controls, Camera& camera, const float delta) -> void
    {
        glm::vec3 moveDirection{};
        if (controls.isPressed(GLFW_KEY_A))
            moveDirection.x -= 1;
        if (controls.isPressed(GLFW_KEY_D))
            moveDirection.x += 1;
        if (controls.isPressed(GLFW_KEY_W))
            moveDirection.z -= 1;
        if (controls.isPressed(GLFW_KEY_S))
            moveDirection.z += 1;
        if (controls.isPressed(GLFW_KEY_LEFT_CONTROL))
            moveDirection.y -= 1;
        if (controls.isPressed(GLFW_KEY_SPACE))
            moveDirection.y += 1;

        if (glm::length2(moveDirection) > 0)
            moveDirection = glm::normalize(moveDirection);

        moveDirection *= m_speed;
        if (controls.isShiftPressed())
            moveDirection *= 2;

        camera.setPosition(camera.getPosition() + (moveDirection * delta));
    }
};

#endif //CAMERACONTROLLER_H
