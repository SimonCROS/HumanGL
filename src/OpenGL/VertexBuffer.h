//
// Created by loumarti on 1/15/25.
//

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

// #include "errorHandling.h" // TODO check if we need this then add in project or delete
#include "glad/gl.h"

class VertexBuffer {
private:
    unsigned int m_rendererID;
public:
    VertexBuffer(const void *data, unsigned int size);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;

    [[nodiscard]] unsigned int getID() const { return m_rendererID; }
};

#endif //VERTEXBUFFER_H
