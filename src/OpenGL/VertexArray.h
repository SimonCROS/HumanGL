//
// Created by loumarti on 1/15/25.
//

#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "glad/gl.h"
#include "VertexBuffer.h"

class VertexBuffer;

class VertexArray {
private:
    unsigned int m_rendererID;

public:
    VertexArray() {
        glGenVertexArrays(1, &m_rendererID);
    };

    ~VertexArray() {
        glDeleteVertexArrays(1, &m_rendererID);
    };

    void addBuffer(const VertexBuffer &vb, int attribute, int size) const;

    void bind() const {
        glBindVertexArray(m_rendererID);
    };

    void unbind() const {
        glBindVertexArray(0);
    };
};



#endif //VERTEXARRAY_H
