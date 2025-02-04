//
// Created by loumarti on 1/15/25.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "Components/Transform.h"
#include "Engine/Object.h"

enum ViewMode
{
    COLOR,
    TEXTURE
};

class Camera final : public EngineComponent
{
private:
    ViewMode m_mode;
    ft_glm::mat4 m_projectionMatrix{};

public:
    explicit Camera(Object& object, uint32_t width, uint32_t height, float fov);

    [[nodiscard]] auto getViewMode() const -> ViewMode { return m_mode; }
    [[nodiscard]] auto projectionMatrix() const -> const ft_glm::mat4& { return m_projectionMatrix; }

    [[nodiscard]] ft_glm::mat4 computeViewMatrix() const;
};


#endif //CAMERA_H
