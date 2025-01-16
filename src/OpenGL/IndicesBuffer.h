//
// Created by loumarti on 1/16/25.
//

#ifndef INDICESBUFFER_H
#define INDICESBUFFER_H

#include "glad/gl.h"

class IndicesBuffer {
private:
    unsigned int m_rendererID;
public:

    IndicesBuffer(const void *data, unsigned int size) {
        glGenBuffers(1, &m_rendererID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    };

    ~IndicesBuffer() {
        glDeleteBuffers(1, &m_rendererID);
    };

    [[nodiscard]] unsigned int getID() const { return m_rendererID; }
};



#endif //INDICESBUFFER_H
