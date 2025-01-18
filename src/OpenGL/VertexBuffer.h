//
// Created by loumarti on 1/15/25.
//

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H
#include <utility>

class VertexBuffer
{
private:
    unsigned int m_rendererID;

public:
    VertexBuffer();
    VertexBuffer(const VertexBuffer&) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept : m_rendererID(std::exchange(other.m_rendererID, 0))
    {
    }

    ~VertexBuffer();

    void bind() const;
    void unbind() const;
    void write(const void* data, unsigned int size);

    [[nodiscard]] unsigned int getID() const { return m_rendererID; }
};

#endif //VERTEXBUFFER_H
