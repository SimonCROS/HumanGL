//
// Created by Simon Cros on 1/17/25.
//

#include "Cube.h"

#include "OpenGL/ShaderProgram.h"
#include "OpenGL/VertexArray.h"

const GLfloat Cube::s_vertex[24] = {
    -1.0f, -1.0f, 1.0f, //  0 : Front-bottom-left
    1.0f, -1.0f, 1.0f, //   1 : Front-bottom-right
    1.0f, 1.0f, 1.0f, //    2 : Front-top-right
    -1.0f, 1.0f, 1.0f, //   3 : Front-top-left
    -1.0f, -1.0f, -1.0f, // 4 : Back-bottom-left
    1.0f, -1.0f, -1.0f, //  5 : Back-bottom-right
    1.0f, 1.0f, -1.0f, //   6 : Back-top-right
    -1.0f, 1.0f, -1.0f, //  7 : Back-top-left
};

const GLuint Cube::s_indices[36] = {
    // Triangle1 // Triangle2
    0, 1, 2, 2, 3, 0, // Front
    6, 5, 4, 4, 7, 6, // Back
    4, 0, 3, 3, 7, 4, // Left
    1, 5, 6, 6, 2, 1, // Right
    3, 2, 6, 6, 7, 3, // Up
    4, 5, 1, 1, 0, 4, // Down
};

const GLfloat Cube::s_colors[24] = {
    1.0f, 0.0f, 0.0f, // Red 0
    0.0f, 1.0f, 0.0f, // Green 1
    0.0f, 0.0f, 1.0f, // Blue 2
    1.0f, 1.0f, 0.0f, // Yellow 3
    1.0f, 0.0f, 1.0f, // Magenta 4
    0.0f, 1.0f, 1.0f, // Cyan 5
    1.0f, 1.0f, 1.0f, // White 6
    0.0f, 0.0f, 0.0f //  Black 7
};

auto Cube::render(VertexArray& vao, ShaderProgram& shader) -> void
{
    m_vertexBuffer.bind();
    m_indicesBuffer.bind();
    vao.addBuffer(m_vertexBuffer, 0, 3);
    vao.addBuffer(m_colorsBuffer, 1, 3);
    shader.setMat4("transform", computeTransformMatrix());
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
}

auto Cube::renderFamily(VertexArray& vao, ShaderProgram& shader) -> void
{
    render(vao, shader);
    for (Mesh* child : children()) {
        if (child) {
            child->render(vao, shader);
        }
    }
}