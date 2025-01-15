//
// Created by loumarti on 1/15/25.
//

#ifndef CAMERA_H
#define CAMERA_H

#include "Window.h"
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>

enum ViewMode {
    COLOR,
    TEXTURE
};

class Camera {
private:
    GLFWwindow *m_window;
    ViewMode m_mode;
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_viewMatrix;
    glm::mat4 m_modelMatrix;
    glm::mat4 m_translationMatrix;
    glm::mat4 m_rotationMatrix;


    // camera
    glm::vec3 m_position = glm::vec3( 0, 0, 5 ); // Initial position : on +Z
    glm::vec3 m_model_position = glm::vec3(0.0f, 0.0f, 0.0f); // relative center of model

    // Model Rotation
    bool m_autoRotate = false;
    float m_lastRotateSwitchTime = 0.0f;
    float m_lastWireframeSwitchTime = 0.0f;
    float m_rotationRate = 2.0f;

    // Model Tranlsation
    const float m_moveRate = 0.1f;
    const float m_shiftRate = 5.0f;

public:
    // Model
    glm::vec3 modelCenter = glm::vec3(0.0f, 0.0f, 0.0f); // absolute center of model

    Camera(GLFWwindow *window);
    ~Camera();

    inline ViewMode     getViewMode() const { return m_mode; }
    inline glm::mat4 getViewMatrix() const { return m_viewMatrix; }
    inline glm::mat4 getProjectionMatrix() const { return m_projectionMatrix; }
    inline glm::mat4 getModelMatrix() const { return m_modelMatrix; }
    bool isShiftPressed() const;
    bool isAltPressed() const;

    glm::mat4 computeMVP() const;
    void moveModelFromInputs();
    void rotateModelFromInputs();
    void selectRotationSpeedFromInputs();
    void switchAutoRotateFromInputs();
    void switchWireframeFromInputs();
    void enableZoom();
    void autoRotate();
    void translateModelToCenter();
    void translateModelWithVec3(const glm::vec3 &vec3);
    void rotateModelWithVec3(const glm::vec3 &vec3);
};



#endif //CAMERA_H
