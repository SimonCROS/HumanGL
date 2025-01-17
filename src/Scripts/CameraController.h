//
// Created by Simon Cros on 1/16/25.
//

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "Camera.h"
#include "Controls.h"
#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

class CameraController {
private:
    float m_distance{5.0f};
    Transform m_target{};

    float pitch{};
    float yaw{};

public:
    auto update(const Controls& controls, Camera& camera, const float delta) -> void
    {
        if (controls.isPressed(GLFW_KEY_A))
            yaw += delta;
        if (controls.isPressed(GLFW_KEY_D))
            yaw -= delta;
        if (controls.isPressed(GLFW_KEY_W))
            pitch += delta;
        if (controls.isPressed(GLFW_KEY_S))
            pitch -= delta;
        if (controls.isPressed(GLFW_KEY_R))
        {
            pitch = 0;
            yaw = 0;
        }

        const auto rotation = glm::quat({pitch, yaw, 0.0f});

        const glm::vec3 forward = rotation * glm::vec3(0.0f, 0.0f, -1.0f);
        const glm::vec3 position = m_target.position() - forward * m_distance;

        camera.transform().position() = position;
        camera.transform().rotation() = rotation;
    }

    // auto rotate(float angle) -> void
    // {
    //     glm::angleAxis(angle, )
    // }
};

#endif //CAMERACONTROLLER_H
