//
// Created by Simon Cros on 26/01/2025.
//

#ifndef MODEL_H
#define MODEL_H

#include <unordered_map>
#include <vector>

#include "MicroGLTF/Struct.h"
#include "Animation.h"
#include "OpenGL/ShaderProgram.h"
#include "OpenGL/VertexArray.h"

class Model
{
private:
    GLuint m_vao;
    std::vector<GLuint> m_buffers;
    std::vector<GLuint> m_textures;
    std::vector<Animation> m_animations;

    ShaderProgram& m_program;
    const microgltf::Model& m_model;

    GLuint m_currentArrayBufferId{0};
    GLuint m_currentElementArrayBufferId{0};
    int m_currentAnimationIndex{-1};

    auto bindTexture(int textureIndex, const std::string_view& bindingKey, GLint bindingValue) -> void; // TMP
    auto renderMesh(int meshIndex, const glm::mat4& transform) -> void;
    auto renderNode(int nodeIndex, glm::mat4 transform) -> void;

public:
    static auto Create(const microgltf::Model& model, ShaderProgram& program) -> Model;

    Model(const GLuint vao, std::vector<GLuint>&& buffers, std::vector<GLuint>&& textures,
          std::vector<Animation>&& animations, ShaderProgram& program, const microgltf::Model& model) :
        m_vao(vao), m_buffers(std::move(buffers)), m_textures(std::move(textures)), m_animations(std::move(animations)), m_program(program), m_model(model)
    {
    }

    auto render() -> void;
};

#endif //MODEL_H
