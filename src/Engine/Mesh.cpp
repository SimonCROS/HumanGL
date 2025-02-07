//
// Created by Simon Cros on 26/01/2025.
//

#include <iostream>

#include "stb_image.h"

#include "HumanGLConfig.h"
#include "Mesh.h"

#include "OpenGL/ShaderProgram.h"

static auto addBuffer(const microgltf::Model& model, const size_t accessorId,
                      std::vector<GLuint>& buffers) -> GLuint
{
    GLuint glBuffer = 0;

    const auto& accessor = model.accessors[accessorId];
    glBuffer = buffers[accessor.bufferView];
    if (glBuffer > 0)
        return glBuffer;

    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];

    if (bufferView.target == 0)
        return 0; // Can be ignored for this project

    glGenBuffers(1, &glBuffer);
    glBindBuffer(bufferView.target, glBuffer);
    glBufferData(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

    buffers[accessor.bufferView] = glBuffer;
    return glBuffer;
}

static auto loadTexture(const std::string& modelFileName, const microgltf::Model& model, const int& textureId,
                        std::vector<GLuint>& textures, const GLint internalFormat) -> void
{
    if (textures[textureId] > 0)
        return;

    const auto& texture = model.textures[textureId];
    assert(texture.source >= 0);

    GLuint glTexture = 0;
    const auto& image = model.images[texture.source];

    if (!image.uri.empty())
    {
        glGenTextures(1, &glTexture);
        glBindTexture(GL_TEXTURE_2D, glTexture);

        if (texture.sampler >= 0)
        {
            const auto& sampler = model.samplers[texture.sampler];

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, sampler.wrapS);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, sampler.wrapT);
            if (sampler.minFilter >= 0)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampler.minFilter);
            if (sampler.magFilter >= 0)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampler.magFilter);
        }

        int width, height, component;
        stbi_uc* data = stbi_load((RESOURCE_PATH"models/" + modelFileName + "/" + image.uri).c_str(), &width, &height,
                                  &component,
                                  0);
        if (data != nullptr)
        {
            GLenum format = GL_RGBA;
            if (component == 1)
                format = GL_RED;
            else if (component == 2)
                format = GL_RG;
            else if (component == 3)
                format = GL_RGB;

            glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cerr << "ERROR::TEXTURE::LOADING_FAILED\n" << stbi_failure_reason() << std::endl;
        }

        stbi_image_free(data);
    }

    textures[textureId] = glTexture;
}

auto Mesh::Create(const std::string& modelFileName, const microgltf::Model& model, ShaderProgram& program) -> Mesh
{
    std::vector<GLuint> buffers;
    std::vector<GLuint> textures;
    std::vector<Animation> animations;
    ModelRenderInfo renderInfo;

    buffers.resize(model.bufferViews.size(), 0);
    textures.resize(model.textures.size(), 0);
    renderInfo.meshes = std::make_unique<MeshRenderInfo[]>(model.meshes.size());
    for (size_t i = 0; i < model.meshes.size(); i++)
    {
        const auto& mesh = model.meshes[i];
        auto& meshRenderInfo = renderInfo.meshes[i];

        renderInfo.meshes[i].primitives = std::make_unique<PrimitiveRenderInfo[]>(mesh.primitives.size());

        for (size_t j = 0; j < mesh.primitives.size(); j++)
        {
            const auto& primitive = mesh.primitives[j];
            auto& primitiveRenderInfo = meshRenderInfo.primitives[j];

            VertexArrayFlags vertexArrayFlags = VertexArrayHasNone;
            ShaderFlags shaderFlags = ShaderHasNone;

            if (primitive.indices >= 0)
                addBuffer(model, primitive.indices, buffers);

            for (const auto& [attributeName, accessorId] : primitive.attributes)
            {
                if (attributeName == "POSITION")
                    vertexArrayFlags |= VertexArrayHasPosition;

                if (attributeName == "NORMAL")
                    vertexArrayFlags |= VertexArrayHasNormal;

                if (attributeName == "COLOR_0")
                {
                    vertexArrayFlags |= VertexArrayHasColor0;
                    if (model.accessors[accessorId].type == microgltf::Vec3)
                        shaderFlags |= ShaderHasVec3Colors;
                    else if (model.accessors[accessorId].type == microgltf::Vec4)
                        shaderFlags |= ShaderHasVec4Colors;
                }

                if (attributeName == "TEXCOORD_0")
                    vertexArrayFlags |= VertexArrayHasTexCoord0;
            }

            if (primitive.material >= 0)
            {
                const auto& material = model.materials[primitive.material];
                if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
                {
                    loadTexture(modelFileName, model, material.pbrMetallicRoughness.baseColorTexture.index, textures,
                                GL_SRGB_ALPHA);
                    shaderFlags |= ShaderHasBaseColorMap;
                }
            }

            primitiveRenderInfo.vertexArrayFlags = vertexArrayFlags;
            primitiveRenderInfo.shaderFlags = shaderFlags;
        }
    }

    animations.reserve(model.animations.size());
    for (const auto& animation : model.animations)
        animations.emplace_back(Animation::Create(model, animation));

    renderInfo.accessors = std::make_unique<AccessorRenderInfo[]>(model.accessors.size());
    for (size_t i = 0; i < model.accessors.size(); i++)
    {
        const auto& accessor = model.accessors[i];
        const auto& bufferView = model.bufferViews[accessor.bufferView];

        auto& accessorRenderInfo = renderInfo.accessors[i];
        accessorRenderInfo.bufferId = addBuffer(model, i, buffers);
        accessorRenderInfo.componentSize = microgltf::getComponentSizeInBytes(accessor.componentType);
        accessorRenderInfo.componentCount = microgltf::getNumComponentsInType(accessor.type);
        accessorRenderInfo.byteStride = bufferView.byteStride > 0
                                            ? static_cast<GLsizei>(bufferView.byteStride)
                                            : accessorRenderInfo.byteStride = accessorRenderInfo.componentSize *
                                            accessorRenderInfo.componentCount;
    }

    return {std::move(buffers), std::move(textures), std::move(animations), std::move(renderInfo), model};
}
