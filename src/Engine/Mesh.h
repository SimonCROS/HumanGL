//
// Created by Simon Cros on 26/01/2025.
//

#ifndef MODEL_H
#define MODEL_H

#include <memory>
#include <vector>

#include "MicroGLTF/Struct.h"
#include "Animation.h"
#include "OpenGL/ShaderProgram.h"
#include "OpenGL/VertexArray.h"

struct PrimitiveRenderInfo
{
    GLuint vao{0};
    GLuint shader{0};
};

struct MeshRenderInfo
{
    std::unique_ptr<PrimitiveRenderInfo[]> primitives{nullptr};
};

struct ModelRenderInfo
{
    std::unique_ptr<MeshRenderInfo[]> meshes{nullptr};
};

class Mesh
{
private:
    std::vector<GLuint> m_buffers;
    std::vector<GLuint> m_textures;
    std::vector<Animation> m_animations; // TODO use a pointer to ensure location never change

    const microgltf::Model& m_model;

public:
    static auto Create(const std::string& modelFileName, const microgltf::Model& model, ShaderProgram& program) -> Mesh;

    Mesh(std::vector<GLuint>&& buffers, std::vector<GLuint>&& textures, std::vector<Animation>&& animations,
         const microgltf::Model& model) :
        m_buffers(std::move(buffers)), m_textures(std::move(textures)), m_animations(std::move(animations)),
        m_model(model)
    {
    }

    [[nodiscard]] auto model() const -> const microgltf::Model& { return m_model; }

    [[nodiscard]] auto buffer(const size_t index) const -> GLuint { return m_buffers[index]; }

    [[nodiscard]] auto texture(const size_t index) const -> GLuint { return m_textures[index]; }

    [[nodiscard]] auto animations() const -> const std::vector<Animation>& { return m_animations; }
};

#endif //MODEL_H
