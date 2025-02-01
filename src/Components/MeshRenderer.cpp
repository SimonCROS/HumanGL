//
// Created by Simon Cros on 1/29/25.
//

#include "MeshRenderer.h"
#include "Engine/Engine.h"

static void* bufferOffset(const size_t offset)
{
    return reinterpret_cast<void*>(offset);
}

auto MeshRenderer::bindTexture(Engine& engine, const int textureIndex, const std::string_view& bindingKey,
                               const GLint bindingValue) -> void
{
    // assert(bindingValue < CUSTOM_MAX_BINDED_TEXTURES);

    const GLuint glTexture = m_mesh.texture(textureIndex);

    // SetInt on program before, if the shader has changed
    m_program.setInt(bindingKey.data(), bindingValue);

    // if (state.bindedTextures[bindingValue] == glTexture)
    // return;

    glActiveTexture(GL_TEXTURE0 + bindingValue);
    glBindTexture(GL_TEXTURE_2D, glTexture > 0 ? glTexture : whiteTexture);
    // state.bindedTextures[bindingValue] = glTexture;
}

auto MeshRenderer::renderMesh(Engine& engine, const int meshIndex, const glm::mat4& transform) -> void
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
                const GLuint bufferId = m_mesh.buffer(accessor.bufferView);
                glBindBuffer(GL_ARRAY_BUFFER, bufferId);

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
            const auto& material = m_mesh.model().materials[primitive.material];

            engine.setDoubleSided(material.doubleSided);

            if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
            {
                bindTexture(engine, material.pbrMetallicRoughness.baseColorTexture.index, "u_baseColorTexture", 0);
                m_program.setVec4("u_baseColorFactor", material.pbrMetallicRoughness.baseColorFactor);
            }

            if (material.normalTexture.index >= 0)
            {
                bindTexture(engine, material.normalTexture.index, "u_normalMap", 2);
                m_program.setFloat("u_normalScale", static_cast<float>(material.normalTexture.scale));
            }
        }
        else
        {
            engine.setDoubleSided(false);
        }

        assert(primitive.indices >= 0); // TODO handle non indexed primitives

        const microgltf::Accessor& indexAccessor = m_mesh.model().accessors[primitive.indices];

        const GLuint bufferId = m_mesh.buffer(indexAccessor.bufferView);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);

        glDrawElements(primitive.mode, static_cast<GLsizei>(indexAccessor.count), indexAccessor.componentType,
                       bufferOffset(indexAccessor.byteOffset));
        // CheckErrors("draw elements");
    }
}

auto MeshRenderer::renderNode(Engine& engine, const int nodeIndex, glm::mat4 transform) -> void
{
    const microgltf::Node& node = m_mesh.model().nodes[nodeIndex];

    if (node.matrix.has_value())
    {
        transform *= *node.matrix;
    }
    else
    {
        const auto& tr = m_animator.has_value()
                             ? m_animator->get().nodeTransform(nodeIndex)
                             : Animator::AnimatedTransform{};

        if (tr.translation.has_value())
            transform = glm::translate(transform, *tr.translation);
        else if (node.translation.has_value())
            transform = glm::translate(transform, *node.translation);

        if (tr.rotation.has_value())
            transform *= glm::mat4_cast(*tr.rotation);
        else if (node.rotation.has_value())
            transform *= glm::mat4_cast(*node.rotation);

        if (tr.scale.has_value())
            transform = glm::scale(transform, *tr.scale);
        else if (node.scale.has_value())
            transform = glm::scale(transform, *node.scale);
    }

    if (node.mesh > -1)
        renderMesh(engine, node.mesh, transform);
    for (const auto childIndex : node.children)
        renderNode(engine, childIndex, transform);
}

void MeshRenderer::onRender(Engine& engine)
{
    m_program.use();
    for (const auto nodeIndex : m_mesh.model().scenes[m_mesh.model().scene].nodes)
        renderNode(engine, nodeIndex, glm::scale(glm::identity<glm::mat4>(), glm::vec3(10)));
}
