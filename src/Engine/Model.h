//
// Created by Simon Cros on 26/01/2025.
//

#ifndef MODEL_H
#define MODEL_H

#include <unordered_map>
#include <vector>

#include "MicroGLTF/Struct.h"
#include "Animation.h"
#include "OpenGL/VertexArray.h"

class Model
{
private:
    int m_vao;
    std::unordered_map<int, GLuint> m_buffers;
    std::unordered_map<int, GLuint> m_textures;
    std::vector<Animation> m_animations;

public:
    static auto Create(const microgltf::Model& model) -> Model;
};

#endif //MODEL_H
