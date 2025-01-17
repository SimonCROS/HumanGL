//
// Created by Simon Cros on 1/16/25.
//

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "Camera.h"
#include "Controls.h"
#include "../../cmake-build-clang-12/_deps/glm-src/glm/gtx/rotate_normalized_axis.inl"
#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/rotate_vector.hpp"

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
    const float m_rotateSpeed = 2.0f;

public:
    auto update(const Controls& controls, Camera& camera, const float delta) -> void
    {
        float speed = m_rotateSpeed * delta;
        auto direction = camera.getDirection();
        if (controls.isPressed(GLFW_KEY_LEFT))
            direction = glm::rotateY(direction, speed);
        if (controls.isPressed(GLFW_KEY_RIGHT))
            direction = glm::rotateY(direction, -speed);
        auto axisRoll = glm::normalize(glm::cross(direction, {0.0f, 1.0f, 0.0f}));
        if (controls.isPressed(GLFW_KEY_UP))
            direction = glm::rotate(direction, speed, axisRoll);
        if (controls.isPressed(GLFW_KEY_DOWN))
            direction = glm::rotate(direction, -speed, axisRoll);
        camera.setDirection(direction);

        glm::vec3 moveDirection{};
        if (controls.isPressed(GLFW_KEY_A))
            moveDirection -= axisRoll;
        if (controls.isPressed(GLFW_KEY_D))
            moveDirection += axisRoll;
        if (controls.isPressed(GLFW_KEY_W))
            moveDirection += direction;
        if (controls.isPressed(GLFW_KEY_S))
            moveDirection -= direction;
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
