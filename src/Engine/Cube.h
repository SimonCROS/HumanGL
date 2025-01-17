//
// Created by Simon Cros on 1/17/25.
//

#ifndef CUBE_H
#define CUBE_H

#include "Mesh.h"
#include "glad/gl.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/IndicesBuffer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/VertexArray.h"

class Cube final : public Mesh {
    static const GLfloat s_vertex[24];
    static const GLuint s_indices[36];
    static const GLfloat s_colors[36];

    // TODO only one vertex/indices buffer cause same for every cubes
    VertexBuffer m_vertexBuffer;
    IndicesBuffer m_indicesBuffer;
    VertexBuffer m_colorsBuffer; // TODO remove, pass to vertex

public:
    Cube()
    {
        m_vertexBuffer.bind();
        m_vertexBuffer.write(s_vertex, sizeof(s_vertex));
        m_vertexBuffer.unbind();
        m_indicesBuffer.bind();
        m_indicesBuffer.write(s_indices, sizeof(s_indices));
        m_indicesBuffer.unbind();
        m_colorsBuffer.bind();
        m_colorsBuffer.write(s_colors, sizeof(s_colors));
        m_colorsBuffer.unbind();
    }

    auto render(VertexArray& vao, Shader& shader) -> void override;
};

#endif //CUBE_H
