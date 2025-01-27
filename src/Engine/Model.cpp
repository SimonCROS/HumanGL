//
// Created by Simon Cros on 26/01/2025.
//

#include "stb_image.h"

#include "HumanGLConfig.h"
#include "Model.h"

static auto addBuffer(const microgltf::Model& model, const int accessorId,
                      std::vector<GLuint>& buffers) -> void
{
    const auto& accessor = model.accessors[accessorId];
    if (buffers[accessor.bufferView] > 0)
        return;

    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];
    GLuint glBuffer = 0;

    glGenBuffers(1, &glBuffer);
    glBindBuffer(bufferView.target, glBuffer);
    glBufferData(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

    buffers[accessor.bufferView] = glBuffer;
}

static auto loadTexture(const microgltf::Model& model, const int& textureId, std::vector<GLuint>& textures,
                        const GLint internalformat) -> void
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
        stbi_uc* data = stbi_load((RESOURCE_PATH"models/iron_golem/" + image.uri).c_str(), &width, &height, &component,
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

            glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
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

static void* bufferOffset(const size_t offset)
{
    return reinterpret_cast<void*>(offset);
}

auto Model::bindTexture(const int textureIndex, const std::string_view& bindingKey, const GLint bindingValue) -> void
{
    // assert(bindingValue < CUSTOM_MAX_BINDED_TEXTURES);

    GLuint glTexture = m_textures[textureIndex];

    // SetInt on program before, if the shader has changed
    m_program.setInt(bindingKey.data(), bindingValue);

    // if (state.bindedTextures[bindingValue] == glTexture)
    // return;

    glActiveTexture(GL_TEXTURE0 + bindingValue);
    glBindTexture(GL_TEXTURE_2D, glTexture > 0 ? glTexture : whiteTexture);
    // state.bindedTextures[bindingValue] = glTexture;
}

auto Model::renderMesh(const int meshIndex, const glm::mat4& transform) -> void
{
    const microgltf::Mesh& mesh = m_model.meshes[meshIndex];

    for (const auto& primitive : mesh.primitives)
    {
        for (const auto& [attribute, accessorIndex] : primitive.attributes)
        {
            const microgltf::Accessor& accessor = m_model.accessors[accessorIndex];

            const int attributeLocation = m_program.getAttributeLocation(attribute);
            if (attributeLocation != -1)
            {
                const GLuint bufferId = m_buffers[accessor.bufferView];
                if (m_currentArrayBufferId != bufferId)
                    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
                m_currentArrayBufferId = bufferId;
                // m_vao.bindArrayBuffer(m_buffers.at(accessor.bufferView));

                const microgltf::BufferView& bufferView = m_model.bufferViews[accessor.bufferView];
                const int componentSize = microgltf::getComponentSizeInBytes(accessor.componentType);
                const int componentCount = microgltf::getNumComponentsInType(accessor.type);

                GLsizei byteStride;
                if (bufferView.byteStride > 0)
                    byteStride = static_cast<GLsizei>(bufferView.byteStride);
                else
                    byteStride = componentSize * componentCount;

                glVertexAttribPointer(attributeLocation, componentCount, accessor.componentType, GL_FALSE, byteStride,
                                      bufferOffset(accessor.byteOffset));
                // CheckErrors("vertex attrib pointer");

                m_program.enableAttribute(attributeLocation);
                // CheckErrors("enable vertex attrib array");
            }
        }

        m_program.applyAttributeChanges();
        m_program.setMat4("u_transform", transform);

        if (primitive.material >= 0)
        {
            const auto &material = m_model.materials[primitive.material];

            // setDoubleSided(material.doubleSided);

            if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
            {
                bindTexture(material.pbrMetallicRoughness.baseColorTexture.index, "u_baseColorTexture", 0);
                m_program.setVec4("u_baseColorFactor", material.pbrMetallicRoughness.baseColorFactor);
            }

            if (material.normalTexture.index >= 0)
            {
                bindTexture(material.normalTexture.index, "u_normalMap", 2);
                m_program.setFloat("u_normalScale", static_cast<float>(material.normalTexture.scale));
            }
        }
        else
        {
            // setDoubleSided(false);
        }

        assert(primitive.indices >= 0); // TODO handle non indexed primitives

        const microgltf::Accessor& indexAccessor = m_model.accessors[primitive.indices];

        const GLuint bufferId = m_buffers[indexAccessor.bufferView];
        if (m_currentElementArrayBufferId != bufferId)
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
        m_currentElementArrayBufferId = bufferId;
        // m_vao.bindElementArrayBuffer(m_buffers.at(accessor.bufferView));

        glDrawElements(primitive.mode, static_cast<GLsizei>(indexAccessor.count), indexAccessor.componentType,
                       bufferOffset(indexAccessor.byteOffset));
        // CheckErrors("draw elements");
    }
}

auto Model::renderNode(const int nodeIndex, glm::mat4 transform) -> void
{
    const microgltf::Node& node = m_model.nodes[nodeIndex];

    if (node.matrix.has_value())
    {
        transform *= *node.matrix;
    }
    else
    {
        // const Animation::AnimatedNode an = animation.animatedNode(nodeIndex);

        /*if (an.translationSampler >= 0)
            transform = glm::translate(transform, animation.sampler(an.translationSampler).vec3());
        else */if (node.translation.has_value())
            transform = glm::translate(transform, *node.translation);

        /*if (an.rotationSampler >= 0)
            transform *= glm::mat4_cast(animation.sampler(an.rotationSampler).quat());
        else */if (node.rotation.has_value())
            transform *= glm::mat4_cast(*node.rotation);

        /*if (an.scaleSampler >= 0)
            transform = glm::scale(transform, animation.sampler(an.scaleSampler).vec3());
        else */if (node.scale.has_value())
            transform = glm::scale(transform, *node.scale);
    }

    if (node.mesh > -1)
        renderMesh(node.mesh, transform);
    for (const auto childIndex : node.children)
        renderNode(childIndex, transform);
}

auto Model::Create(const microgltf::Model& model, ShaderProgram& program) -> Model
{
    GLuint vao{0};
    std::vector<GLuint> buffers;
    std::vector<GLuint> textures;
    std::vector<Animation> animations;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    buffers.resize(model.bufferViews.size(), 0);
    textures.resize(model.textures.size(), 0);
    for (const auto& mesh : model.meshes)
    {
        for (const auto& primitive : mesh.primitives)
        {
            if (primitive.indices >= 0)
                addBuffer(model, primitive.indices, buffers);

            for (const auto& [attributeName, accessorId] : primitive.attributes)
                addBuffer(model, accessorId, buffers);

            if (primitive.material >= 0)
            {
                const auto& material = model.materials[primitive.material];
                if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
                    loadTexture(model, material.pbrMetallicRoughness.baseColorTexture.index, textures, GL_SRGB_ALPHA);
                if (material.normalTexture.index >= 0)
                    loadTexture(model, material.normalTexture.index, textures, GL_RGB);
            }
        }
    }

    animations.reserve(model.animations.size());
    for (const auto& animation : model.animations)
        animations.emplace_back(Animation::create(model, animation));

    return {vao, std::move(buffers), std::move(textures), std::move(animations), program, model};
}

auto Model::render() -> void
{
    for (const auto nodeIndex : m_model.scenes[m_model.scene].nodes)
        renderNode(nodeIndex, glm::scale(glm::identity<glm::mat4>(), glm::vec3(10)));
}
