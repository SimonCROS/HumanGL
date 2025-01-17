//
// Created by loumarti on 1/16/25.
//

#ifndef CUBOID_H
#define CUBOID_H

#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad/gl.h"
#include <array>

class Cuboid {
private:
    glm::mat4 m_modelMatrix;
    GLfloat m_vertex_buffer[24];

public:
    explicit Cuboid() noexcept : m_modelMatrix(glm::mat4(1.0f)) {
        std::copy(std::begin(Cuboid::s_cube_vertex_buffer), std::end(Cuboid::s_cube_vertex_buffer), m_vertex_buffer);
    }

    ~Cuboid() = default;

    // todo : build on class (raii, move/copy etc.)

    [[nodiscard]] auto getModel() const -> const glm::mat4 & {
        return m_modelMatrix;
    }

    [[nodiscard]] auto getVertexBuffer() const -> const GLfloat * {
        return m_vertex_buffer;
    }

    void scaleVertexBuffer(glm::vec3 scaling) noexcept;

    void translate(glm::vec3 translation) {
        m_modelMatrix = glm::translate(m_modelMatrix, translation);
    }

    static const GLfloat s_cube_vertex_buffer[24];
    static const GLuint s_indices_buffer[36];
    static const GLfloat s_color_buffer[36];
};


#endif //CUBOID_H
