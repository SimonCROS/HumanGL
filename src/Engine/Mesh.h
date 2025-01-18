//
// Created by Simon Cros on 18/01/2025.
//

#ifndef MESH_H
#define MESH_H

#include <cstdint>

#include "glad/gl.h"
#include "OpenGL/IndicesBuffer.h"
#include "OpenGL/VertexBuffer.h"

class Mesh
{
private:
    static const GLfloat s_cube_vertex[24];
    static const GLuint s_cube_indices[36];
    static const GLfloat s_cube_colors[36];

    friend class MeshComponent;

    VertexBuffer m_vertexBuffer;
    IndicesBuffer m_indicesBuffer;
    VertexBuffer m_colorsBuffer;

    uint32_t m_vertexCount{};
    uint32_t m_indicesCount{};

    Mesh() = default;

public:
    Mesh(const GLfloat* vertex, size_t vertexCount, const GLuint* indices, size_t indicesCount, const GLfloat* colors,
         size_t colorsCount);

    static auto Cube() -> Mesh
    {
        return {s_cube_vertex, 24, s_cube_indices, 36, s_cube_colors, 36};
    }

    [[nodiscard]] auto vertexCount() const -> uint32_t { return m_vertexCount; }
    [[nodiscard]] auto indicesCount() const -> uint32_t { return m_indicesCount; }
};

#endif //MESH_H
