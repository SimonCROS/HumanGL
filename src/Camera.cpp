//
// Created by loumarti on 1/15/25.
//

#include "Camera.h"

Camera::Camera(const uint32_t width, const uint32_t height, const float fov) : m_mode(COLOR)
{
    const float aspect = static_cast<float>(width) / static_cast<float>(height);

    m_projectionMatrix = ft_glm::perspective(ft_glm::radians(fov), aspect, 0.1f, 100.0f);
}

auto Camera::computeViewMatrix() const -> ft_glm::mat4
{
    const ft_glm::vec3 forward = m_transform.rotation() * ft_glm::vec3(0.0f, 0.0f, -1.0f);
    const ft_glm::vec3 up = m_transform.rotation() * ft_glm::vec3(0.0f, 1.0f, 0.0f);
    const ft_glm::vec3 center = m_transform.position() + forward;
    return ft_glm::lookAt(m_transform.position(), center, up);
}
