//
// Created by loumarti on 1/16/25.
//

#include "Cuboid.h"

const GLfloat Cuboid::s_cube_vertex_buffer[24] = {
    -1.0f, -1.0f, 1.0f, // 0 : Front-bottom-left
    1.0f, -1.0f, 1.0f, // 1 : Front-bottom-right
    1.0f, 1.0f, 1.0f, // 2 : Front-top-right
    -1.0f, 1.0f, 1.0f, // 3 : Front-top-left
    -1.0f, -1.0f, -1.0f, // 4 : Back-bottom-left
    1.0f, -1.0f, -1.0f, // 5 : Back-bottom-right
    1.0f, 1.0f, -1.0f, // 6 : Back-top-right
    -1.0f, 1.0f, -1.0f, // 7 : Back-top-left
};

const GLuint Cuboid::s_indices_buffer[36] = {
    // Triangle1 // Triangle2
    0, 1, 2, 2, 3, 0, // Front
    6, 5, 4, 4, 7, 6, // Back
    4, 0, 3, 3, 7, 4, // Left
    1, 5, 6, 6, 2, 1, // Right
    3, 2, 6, 6, 7, 3, // Up
    4, 5, 1, 1, 0, 4, // Down
};

const GLfloat Cuboid::s_color_buffer[36] = {
    1.0f, 0.0f, 0.0f, // Red 0
    0.0f, 1.0f, 0.0f, // Green 1
    0.0f, 0.0f, 1.0f, // Blue 2
    1.0f, 1.0f, 0.0f, // Yellow 3
    1.0f, 0.0f, 1.0f, // Magenta 4
    0.0f, 1.0f, 1.0f, // Cyan 5
    1.0f, 1.0f, 1.0f, // White 6
    0.0f, 0.0f, 0.0f // Black 7
};

void Cuboid::scaleVertexBuffer(const glm::vec3 scaling) noexcept {
    for (int i = 0; i < 8; ++i) {
        m_vertex_buffer[i * 3 + 0] *= scaling.x;
        m_vertex_buffer[i * 3 + 1] *= scaling.y;
        m_vertex_buffer[i * 3 + 2] *= scaling.z;
    }
}
