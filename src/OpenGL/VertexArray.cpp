//
// Created by loumarti on 1/15/25.
//

#include "VertexArray.h"

void VertexArray::addBuffer(const VertexBuffer &vb, int attribute, int size) const {
    bind();
    vb.bind();
    glEnableVertexAttribArray(attribute);

    glVertexAttribPointer(
        attribute,          // attribute 0 linked to layout location in shader.
        size,               // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride // will be optimised with use of a bufferLayout in bigger project
        nullptr            // array buffer offset // same as above
    );
}
