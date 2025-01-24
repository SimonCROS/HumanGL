//
// Created by loumarti on 1/15/25.
//

#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "glad/gl.h"
#include "VertexBuffer.h"

class VertexBuffer;

class VertexArray
{
private:
    GLuint m_id;
    GLuint m_currentArrayBufferId{0};
    GLuint m_currentElementArrayBufferId{0};

public:
    VertexArray()
    {
        glGenVertexArrays(1, &m_id);
    };

    ~VertexArray()
    {
        glDeleteVertexArrays(1, &m_id);
    };

    void bind() const
    {
        glBindVertexArray(m_id);
    };

    void unbind() const
    {
        glBindVertexArray(0);
    }

    void bindArrayBuffer(const GLuint id)
    {
        if (m_currentArrayBufferId != id)
        {
            glBindBuffer(GL_ARRAY_BUFFER, id);
        }
        m_currentArrayBufferId = id;
    }

    void unbindArrayBuffer()
    {
        bindArrayBuffer(0);
    }

    void bindElementArrayBuffer(const GLuint id)
    {
        if (m_currentElementArrayBufferId != id)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
        }
        m_currentElementArrayBufferId = id;
    }

    void unbindElementArrayBuffer()
    {
        bindArrayBuffer(0);
    }
};


#endif //VERTEXARRAY_H
