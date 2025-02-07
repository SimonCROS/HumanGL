//
// Created by Simon Cros on 1/29/25.
//

#include "MeshRenderer.h"
#include "Engine/Engine.h"
#include "Engine/Object.h"

static void* bufferOffset(const size_t offset)
{
    return reinterpret_cast<void*>(offset);
}

auto MeshRenderer::renderMesh(Engine& engine, const int meshIndex, const ft_glm::mat4& transform) -> void
{
    const auto& mesh = m_mesh.model().meshes[meshIndex];
    const auto& meshRenderInfo = m_mesh.renderInfo().meshes[meshIndex];

    for (int p = 0; p < mesh.primitives.size(); ++p)
    {
        const auto& primitive = mesh.primitives[p];
        const auto& primitiveRenderInfo = meshRenderInfo.primitives[p];

        auto& program = m_program.get().getProgram(primitiveRenderInfo.shaderFlags);
        engine.useProgram(program);

        auto& vertexArray = engine.getVertexArray(primitiveRenderInfo.vertexArrayFlags);
        engine.bindVertexArray(vertexArray);

        for (const auto& [attribute, accessorIndex] : primitive.attributes)
        {
            const auto& accessor = m_mesh.model().accessors[accessorIndex];
            const auto& accessorRenderInfo = m_mesh.renderInfo().accessors[accessorIndex];

            const int attributeLocation = VertexArray::getAttributeLocation(attribute);
            if (attributeLocation != -1)
            {
                vertexArray.bindArrayBuffer(accessorRenderInfo.bufferId);
                glVertexAttribPointer(attributeLocation,
                                      accessorRenderInfo.componentCount,
                                      accessor.componentType,
                                      GL_FALSE,
                                      accessorRenderInfo.byteStride,
                                      bufferOffset(accessor.byteOffset));
            }
        }

        program.setMat4("u_transform", transform);

        if (primitive.material >= 0)
        {
            const auto& material = m_mesh.model().materials[primitive.material];

            engine.setDoubleSided(material.doubleSided);

            if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
            {
                engine.bindTexture(0, m_mesh.texture(material.pbrMetallicRoughness.baseColorTexture.index));
                program.setInt("u_baseColorTexture", 0);
                program.setVec4("u_baseColorFactor", material.pbrMetallicRoughness.baseColorFactor);
            }

            if (material.normalTexture.index >= 0)
            {
                engine.bindTexture(1, m_mesh.texture(material.normalTexture.index));
                program.setInt("u_normalMap", 1);
                program.setFloat("u_normalScale", static_cast<float>(material.normalTexture.scale));
            }
        }
        else
        {
            engine.setDoubleSided(false);
        }

        assert(primitive.indices >= 0); // TODO handle non indexed primitives

        const microgltf::Accessor& indexAccessor = m_mesh.model().accessors[primitive.indices];

        const GLuint bufferId = m_mesh.buffer(indexAccessor.bufferView);
        vertexArray.bindElementArrayBuffer(bufferId);

        glDrawElements(primitive.mode, static_cast<GLsizei>(indexAccessor.count), indexAccessor.componentType,
                       bufferOffset(indexAccessor.byteOffset));
        // CheckErrors("draw elements");
    }
}

auto MeshRenderer::renderNode(Engine& engine, const int nodeIndex, ft_glm::mat4 transform) -> void
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
            transform = ft_glm::translate(transform, *tr.translation);
        else if (node.translation.has_value())
            transform = ft_glm::translate(transform, *node.translation);

        if (tr.rotation.has_value())
            transform *= ft_glm::mat4_cast(*tr.rotation);
        else if (node.rotation.has_value())
            transform *= ft_glm::mat4_cast(*node.rotation);

        if (tr.scale.has_value())
            transform = ft_glm::scale(transform, *tr.scale);
        else if (node.scale.has_value())
            transform = ft_glm::scale(transform, *node.scale);
    }

    transform = ft_glm::scale(transform, m_scaleMultiplier[nodeIndex]);

    if (node.mesh > -1)
        renderMesh(engine, node.mesh, transform);
    for (const auto childIndex : node.children)
        renderNode(engine, childIndex, transform);
}

void MeshRenderer::onRender(Engine& engine)
{
    for (const auto nodeIndex : m_mesh.model().scenes[m_mesh.model().scene].nodes)
        renderNode(engine, nodeIndex, object().transform().trs());
}
