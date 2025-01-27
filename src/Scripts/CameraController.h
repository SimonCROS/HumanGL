//
// Created by Simon Cros on 1/16/25.
//

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

#include "Engine/Engine.h"
#include "Camera.h"
#include "Window/Controls.h"
#include "Engine/EngineComponent.h"
#include "glm/glm.hpp"
#include "glm/gtx/norm.hpp"

class CameraController final : public EngineComponent {
private:
    glm::vec3 m_target{};
    float m_distance{10.0f};

    float pitch{};
    float yaw{};

public:
    CameraController(const glm::vec3 target, const float distance) : m_target(target), m_distance(distance)
    {
    }

    auto onUpdate(Engine& engine) -> void override
    {
        const Controls controls = engine.controls();
        const float delta = engine.frameInfo().deltaTime.count();

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
        const glm::vec3 position = m_target - forward * m_distance;

        engine.camera().transform().position() = position;
        engine.camera().transform().rotation() = rotation;
    }
};

#endif //CAMERACONTROLLER_H
