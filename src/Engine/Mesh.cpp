//
// Created by Simon Cros on 18/01/2025.
//

#include "Mesh.h"

#include "glad/gl.h"

const GLfloat Mesh::s_cube_vertex[24] = {
    -1.0f, -1.0f, 1.0f, //  0 : Front-bottom-left
    1.0f, -1.0f, 1.0f, //   1 : Front-bottom-right
    1.0f, 1.0f, 1.0f, //    2 : Front-top-right
    -1.0f, 1.0f, 1.0f, //   3 : Front-top-left
    -1.0f, -1.0f, -1.0f, // 4 : Back-bottom-left
    1.0f, -1.0f, -1.0f, //  5 : Back-bottom-right
    1.0f, 1.0f, -1.0f, //   6 : Back-top-right
    -1.0f, 1.0f, -1.0f, //  7 : Back-top-left
};

const GLuint Mesh::s_cube_indices[36] = {
    // Triangle1 // Triangle2
    0, 1, 2, 2, 3, 0, // Front
    6, 5, 4, 4, 7, 6, // Back
    4, 0, 3, 3, 7, 4, // Left
    1, 5, 6, 6, 2, 1, // Right
    3, 2, 6, 6, 7, 3, // Up
    4, 5, 1, 1, 0, 4, // Down
};

const GLfloat Mesh::s_cube_colors[36] = {
    1.0f, 0.0f, 0.0f, // Red 0
    0.0f, 1.0f, 0.0f, // Green 1
    0.0f, 0.0f, 1.0f, // Blue 2
    1.0f, 1.0f, 0.0f, // Yellow 3
    1.0f, 0.0f, 1.0f, // Magenta 4
    0.0f, 1.0f, 1.0f, // Cyan 5
    1.0f, 1.0f, 1.0f, // White 6
    0.0f, 0.0f, 0.0f //  Black 7
};

Mesh::Mesh(const float* vertex, size_t vertexCount, const uint32_t* indices, size_t indicesCount,
    const uint32_t* colors, size_t colorsCount)
{
    m_vertexCount = vertexCount;
    m_indicesCount = indicesCount;

    m_vertexBuffer.bind();
    m_vertexBuffer.write(vertex, vertexCount * sizeof(float));
    m_vertexBuffer.unbind();
    m_indicesBuffer.bind();
    m_indicesBuffer.write(indices, indicesCount * sizeof(float));
    m_indicesBuffer.unbind();
    m_colorsBuffer.bind();
    m_colorsBuffer.write(colors, colorsCount * sizeof(float));
    m_colorsBuffer.unbind();
}
