//
// Created by Simon Cros on 1/29/25.
//

#include "MeshRenderer.h"

static void* bufferOffset(const size_t offset)
{
    return reinterpret_cast<void*>(offset);
}

auto MeshRenderer::bindTexture(const int textureIndex, const std::string_view& bindingKey, const GLint bindingValue) -> void
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

auto MeshRenderer::renderMesh(const int meshIndex, const glm::mat4& transform) -> void
{
    const microgltf::Mesh& mesh = m_mesh.model().meshes[meshIndex];

    for (const auto& primitive : mesh.primitives)
    {
        for (const auto& [attribute, accessorIndex] : primitive.attributes)
        {
            const microgltf::Accessor& accessor = m_mesh.model().accessors[accessorIndex];

            const int attributeLocation = m_program.getAttributeLocation(attribute);
            if (attributeLocation != -1)
            {
                const GLuint bufferId = m_buffers[accessor.bufferView];
                if (m_currentArrayBufferId != bufferId)
                    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
                m_currentArrayBufferId = bufferId;
                // m_vao.bindArrayBuffer(m_buffers.at(accessor.bufferView));

                const microgltf::BufferView& bufferView = m_mesh.model().bufferViews[accessor.bufferView];
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
            const auto &material = m_mesh.model().materials[primitive.material];

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

        const microgltf::Accessor& indexAccessor = m_mesh.model().accessors[primitive.indices];

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

auto MeshRenderer::renderNode(const int nodeIndex, glm::mat4 transform) -> void
{
    const microgltf::Node& node = m_mesh.model().nodes[nodeIndex];

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

void MeshRenderer::onRender(Engine& engine)
{
    for (const auto nodeIndex : m_mesh.model().scenes[m_mesh.model().scene].nodes)
        renderNode(nodeIndex, glm::scale(glm::identity<glm::mat4>(), glm::vec3(10)));
}
