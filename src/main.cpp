#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "HumanGLConfig.h"
#include "Camera.h"
#include "Engine.h"
#include "Window.h"
#include "WindowContext.h"
#include "MicroGLTF/Model.h"
#include "OpenGL/ShaderProgramVariants.h"
#include "OpenGL/VertexArray.h"
#include "Scripts/CameraController.h"

void* bufferOffset(const size_t offset)
{
    return reinterpret_cast<void*>(offset);
}

auto renderMesh(const microgltf::Model& model, const int meshIndex, ShaderProgram& program,
                const std::unordered_map<size_t, GLuint>& buffers, const glm::mat4& transform) -> void
{
    const microgltf::Mesh& mesh = model.meshes[meshIndex];

    for (const auto& primitive : mesh.primitives)
    {
        for (const auto& [attribute, accessorIndex] : primitive.attributes)
        {
            const microgltf::Accessor& accessor = model.accessors[accessorIndex];

            const int attributeLocation = program.getAttributeLocation(attribute);
            if (attributeLocation != -1)
            {
                glBindBuffer(GL_ARRAY_BUFFER, buffers.at(accessor.bufferView));

                const microgltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
                const int componentSize = microgltf::GetComponentSizeInBytes(accessor.componentType);
                const int componentCount = microgltf::GetNumComponentsInType(accessor.type);

                GLsizei byteStride;
                if (bufferView.byteStride > 0)
                    byteStride = static_cast<GLsizei>(bufferView.byteStride);
                else
                    byteStride = componentSize * componentCount;

                glVertexAttribPointer(attributeLocation, componentCount, accessor.componentType, GL_FALSE, byteStride,
                                      bufferOffset(accessor.byteOffset));
                // CheckErrors("vertex attrib pointer");

                program.enableAttribute(attributeLocation);
                // CheckErrors("enable vertex attrib array");
            }
        }

        program.applyAttributeChanges();
        program.setMat4("transform", transform);

        const microgltf::Accessor& indexAccessor = model.accessors[primitive.indices];

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.at(indexAccessor.bufferView));

        glDrawElements(primitive.mode, static_cast<GLsizei>(indexAccessor.count), indexAccessor.componentType,
                       bufferOffset(indexAccessor.byteOffset));
        // CheckErrors("draw elements");
    }
}

auto renderNode(const microgltf::Model& model, const int nodeIndex, ShaderProgram& program,
                const std::unordered_map<size_t, GLuint>& buffers, glm::mat4 transform) -> void
{
    const microgltf::Node& node = model.nodes[nodeIndex];

    if (node.matrix.has_value())
    {
        transform *= *node.matrix;
    }
    else
    {
        if (node.translation.has_value())
            transform = glm::translate(transform, *node.translation);

        if (node.rotation.has_value())
            transform *= glm::mat4_cast(*node.rotation);

        if (node.scale.has_value())
            transform = glm::scale(transform, *node.scale);
    }

    if (node.mesh > -1)
        renderMesh(model, node.mesh, program, buffers, transform);
    for (const auto childIndex : node.children)
        renderNode(model, childIndex, program, buffers, transform);
}

#include <fstream>

std::vector<uint8_t> readFileToVector(const std::string& filename, const std::streamsize fileSize)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to open file");
    }

    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    if (!file)
    {
        throw std::runtime_error("Failed to read file");
    }

    return buffer;
}

auto prepare(const microgltf::Model& model, VertexArray& vao, std::unordered_map<size_t, GLuint>& buffers) -> void
{
    vao.bind();

    for (const auto& mesh : model.meshes)
    {
        for (const auto& primitive : mesh.primitives)
        {
            if (primitive.indices < 0)
                continue;

            for (const auto& [attributeName, accessorId] : primitive.attributes)
            {
                const auto& accessor = model.accessors[accessorId];
                const auto& bufferView = model.bufferViews[accessor.bufferView];

                if (!buffers.contains(accessor.bufferView))
                {
                    const auto& buffer = model.buffers[bufferView.buffer];
                    GLuint glBuffer = 0;

                    glGenBuffers(1, &glBuffer);
                    glBindBuffer(bufferView.target, glBuffer);
                    glBufferData(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset,
                                 GL_STATIC_DRAW);

                    buffers[accessor.bufferView] = glBuffer;
                }
            }

            {
                const auto& accessor = model.accessors[primitive.indices];
                if (!buffers.contains(accessor.bufferView))
                {
                    const auto& bufferView = model.bufferViews[accessor.bufferView];
                    const auto& buffer = model.buffers[bufferView.buffer];
                    GLuint glBuffer = 0;

                    glGenBuffers(1, &glBuffer);
                    glBindBuffer(bufferView.target, glBuffer);
                    glBufferData(bufferView.target, static_cast<GLsizeiptr>(bufferView.byteLength),
                                 &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

                    buffers[accessor.bufferView] = glBuffer;
                }
            }
        }
    }

    vao.unbind();
}

auto start() -> Expected<void, std::string>
{
    std::cout << "HumanGL " << HumanGL_VERSION_MAJOR << "." << HumanGL_VERSION_MINOR << std::endl;

    auto e_window_context = WindowContext::Create(4, 1);
    if (!e_window_context)
        return Unexpected("Failed to create window context: " + std::move(e_window_context).error());

    auto e_window = Window::Create(WIDTH, HEIGHT, "HumanGL");
    if (!e_window)
        return Unexpected("Failed to create window: " + std::move(e_window).error());

    auto engine = Engine(*std::move(e_window));
    engine.getWindow().setKeyCallback([](const Window& window, const int key, const int action, int mode) -> void
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            window.setShouldClose();
    });

    auto camera = Camera(engine.getWindow().width(), engine.getWindow().height(), 60.0f);

    microgltf::Model model = {
        .accessors = {
            {.bufferView = 3, .componentType = 5126, .count = 8046, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 96552, .componentType = 5126, .count = 8046, .type = microgltf::Vec3},
            {.bufferView = 4, .componentType = 5126, .count = 8046, .type = microgltf::Vec4},
            {.bufferView = 2, .componentType = 5126, .count = 8046, .type = microgltf::Vec2},
            {.bufferView = 1, .componentType = 5125, .count = 43926, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 193104, .componentType = 5126, .count = 10938, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 324360, .componentType = 5126, .count = 10938, .type = microgltf::Vec3},
            {.bufferView = 4, .byteOffset = 128736, .componentType = 5126, .count = 10938, .type = microgltf::Vec4},
            {.bufferView = 2, .byteOffset = 64368, .componentType = 5126, .count = 10938, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 175704, .componentType = 5125, .count = 58098, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 455616, .componentType = 5126, .count = 5450, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 521016, .componentType = 5126, .count = 5450, .type = microgltf::Vec3},
            {.bufferView = 4, .byteOffset = 303744, .componentType = 5126, .count = 5450, .type = microgltf::Vec4},
            {.bufferView = 2, .byteOffset = 151872, .componentType = 5126, .count = 5450, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 408096, .componentType = 5125, .count = 25884, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 586416, .componentType = 5126, .count = 2084, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 611424, .componentType = 5126, .count = 2084, .type = microgltf::Vec3},
            {.bufferView = 4, .byteOffset = 390944, .componentType = 5126, .count = 2084, .type = microgltf::Vec4},
            {.bufferView = 2, .byteOffset = 195472, .componentType = 5126, .count = 2084, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 511632, .componentType = 5125, .count = 11886, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 636432, .componentType = 5126, .count = 3618, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 679848, .componentType = 5126, .count = 3618, .type = microgltf::Vec3},
            {.bufferView = 4, .byteOffset = 424288, .componentType = 5126, .count = 3618, .type = microgltf::Vec4},
            {.bufferView = 2, .byteOffset = 212144, .componentType = 5126, .count = 3618, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 559176, .componentType = 5125, .count = 16434, .type = microgltf::Scalar},
            {.bufferView = 5, .componentType = 5126, .count = 92, .type = microgltf::Mat4},
            {.bufferView = 0, .componentType = 5123, .count = 8046, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 482176, .componentType = 5126, .count = 8046, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 64368, .componentType = 5123, .count = 10938, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 610912, .componentType = 5126, .count = 10938, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 151872, .componentType = 5123, .count = 5450, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 785920, .componentType = 5126, .count = 5450, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 195472, .componentType = 5123, .count = 2084, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 873120, .componentType = 5126, .count = 2084, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 212144, .componentType = 5123, .count = 3618, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 906464, .componentType = 5126, .count = 3618, .type = microgltf::Vec4},
            {.bufferView = 6, .componentType = 5126, .count = 124, .type = microgltf::Scalar},
            {.bufferView = 7, .componentType = 5126, .count = 124, .type = microgltf::Vec3},
            {.bufferView = 6, .byteOffset = 496, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 1020, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 2096, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 1544, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 4192, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 2072, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 6304, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 2600, .componentType = 5126, .count = 114, .type = microgltf::Scalar},
            {.bufferView = 7, .byteOffset = 1488, .componentType = 5126, .count = 114, .type = microgltf::Vec3},
            {.bufferView = 6, .byteOffset = 3056, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 8416, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 3580, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 10512, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 4104, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 12608, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 4628, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 14704, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 5152, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 16800, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 5676, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 18896, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 6200, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 20992, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 6724, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 23088, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 7248, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 25184, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 7772, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 27280, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 8296, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 29376, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 8820, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 31472, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 9344, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 33568, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 9868, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 35664, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 10392, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 37760, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 10916, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 39856, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 11440, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 41952, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 11964, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 44048, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 12488, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 46144, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 13012, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 48240, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 13536, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 50336, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 14060, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 52432, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 14584, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 54528, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 15108, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 56624, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 15632, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 58720, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 16156, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 60816, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 16684, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 62928, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 17212, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 65040, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 17740, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 67152, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 18268, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 69264, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 18792, .componentType = 5126, .count = 117, .type = microgltf::Scalar},
            {.bufferView = 7, .byteOffset = 2856, .componentType = 5126, .count = 117, .type = microgltf::Vec3},
            {.bufferView = 6, .byteOffset = 19260, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 71360, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 19784, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 73456, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 20308, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 75552, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 20832, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 77648, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 21356, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 79744, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 21880, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 81840, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 22404, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 83936, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 22928, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 86032, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 23452, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 88128, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 23976, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 90224, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 24500, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 92320, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 25024, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 94416, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 25548, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 96512, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 26072, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 98608, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 26596, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 100704, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 27120, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 102800, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 27644, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 104896, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 28168, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 106992, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 28692, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 109088, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 29216, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 111184, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 29740, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 113280, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 30264, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 115376, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 30788, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 117472, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 31312, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 119568, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 31836, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 121664, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 32364, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 123776, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 32892, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 125888, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 33420, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 128000, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 33948, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 130112, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 34476, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 132224, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 35004, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 134336, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 35532, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 136448, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 36056, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 138544, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 36580, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 140640, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 37108, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 142752, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 37636, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 144864, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 38164, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 146976, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 38692, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 149088, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 39216, .componentType = 5126, .count = 131, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 151184, .componentType = 5126, .count = 131, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 39740, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 153280, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 40268, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 155392, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 40796, .componentType = 5126, .count = 32, .type = microgltf::Scalar},
            {.bufferView = 7, .byteOffset = 4260, .componentType = 5126, .count = 32, .type = microgltf::Vec3},
            {.bufferView = 6, .byteOffset = 40924, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 157504, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 41452, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 159616, .componentType = 5126, .count = 132, .type = microgltf::Vec4},
            {.bufferView = 6, .byteOffset = 41980, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 7, .byteOffset = 4644, .componentType = 5126, .count = 132, .type = microgltf::Vec3},
            {.bufferView = 6, .byteOffset = 42508, .componentType = 5126, .count = 132, .type = microgltf::Scalar},
            {.bufferView = 8, .byteOffset = 161728, .componentType = 5126, .count = 132, .type = microgltf::Vec4}
        },
        .bufferViews = {
            {.buffer = 0, .byteLength = 241088, .byteStride = 8, .target = 34962},
            {.buffer = 0, .byteLength = 624912, .byteOffset = 241088, .target = 34963},
            {.buffer = 0, .byteLength = 241088, .byteOffset = 866000, .byteStride = 8, .target = 34962},
            {.buffer = 0, .byteLength = 723264, .byteOffset = 1107088, .byteStride = 12, .target = 34962},
            {.buffer = 0, .byteLength = 964352, .byteOffset = 1830352, .byteStride = 16, .target = 34962},
            {.buffer = 0, .byteLength = 5888, .byteOffset = 2794704},
            {.buffer = 0, .byteLength = 43036, .byteOffset = 2800592},
            {.buffer = 0, .byteLength = 6228, .byteOffset = 2843628, .byteStride = 12},
            {.buffer = 0, .byteLength = 163840, .byteOffset = 2849856, .byteStride = 16}
        },
        .buffers = {{.byteLength = 3013696, .uri = RESOURCE_PATH"models/ancient/scene.bin"}},
        .meshes = {
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 26}, {"NORMAL", 1}, {"POSITION", 0}, {"TANGENT", 2}, {"TEXCOORD_0", 3},
                            {"TEXCOORD_1", 3}, {"WEIGHTS_0", 27}
                        },
                        .indices = 4, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 28}, {"NORMAL", 6}, {"POSITION", 5}, {"TANGENT", 7}, {"TEXCOORD_0", 8},
                            {"TEXCOORD_1", 8}, {"WEIGHTS_0", 29}
                        },
                        .indices = 9, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 30}, {"NORMAL", 11}, {"POSITION", 10}, {"TANGENT", 12}, {"TEXCOORD_0", 13},
                            {"TEXCOORD_1", 13}, {"WEIGHTS_0", 31}
                        },
                        .indices = 14, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 32}, {"NORMAL", 16}, {"POSITION", 15}, {"TANGENT", 17}, {"TEXCOORD_0", 18},
                            {"TEXCOORD_1", 18}, {"WEIGHTS_0", 33}
                        },
                        .indices = 19, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 34}, {"NORMAL", 21}, {"POSITION", 20}, {"TANGENT", 22}, {"TEXCOORD_0", 23},
                            {"TEXCOORD_1", 23}, {"WEIGHTS_0", 35}
                        },
                        .indices = 24, .mode = 4
                    }
                }
            }
        },
        .nodes = {
            {
                .children = {1},
                .matrix = {{1, 0, 0, 0, 0, 2.220446049250313e-16, -1, 0, 0, 1, 2.220446049250313e-16, 0, 0, 0, 0, 1}}
            },
            {
                .children = {2},
                .matrix = {
                    {
                        0.009999999776482582, 0, 0, 0, 0, 0, 0.009999999776482582, 0, 0, -0.009999999776482582, 0, 0, 0,
                        0, 0, 1
                    }
                }
            },
            {.children = {3}}, {.children = {4, 108, 109, 110, 111, 112}},
            {.children = {5}, .rotation = {{1, 8.146033536604591e-8, 0, 0}}},
            {.children = {6, 99, 101, 103, 105, 107, 98, 100, 102, 104, 106}}, {.children = {7}},
            {
                .children = {8, 84, 91},
                .rotation = {{0.8020215630531311, -0.07332886010408401, -0.5887497067451477, -0.06897906213998795}},
                .translation = {{-7.36585807800293, 176.44627380371094, -0.4366716146469116}}
            },
            {
                .children = {9},
                .rotation = {{0.9991529583930969, -0.030044786632061005, 0.027738718315958977, -0.004611248150467873}},
                .scale = {{0.9999999403953552, 0.9999999403953552, 0.9999999403953552}},
                .translation = {{-5.960464477539062e-7, 12.762503623962402, -1.5210342407226562}}
            },
            {
                .children = {10},
                .rotation = {{0.9969561100006104, 0.058689288794994354, 0.04180344194173813, 0.029775282368063927}},
                .scale = {{0.9999999403953552, 1.0000001192092896, 1.000000238418579}},
                .translation = {{-9.5367431640625e-7, 14.994948387145996, 0.0000050067901611328125}}
            },
            {
                .children = {11, 16, 50},
                .rotation = {{0.9969561696052551, 0.05917864292860031, 0.04115712270140648, 0.02970854938030243}},
                .scale = {{0.9999998807907104, 0.9999998807907104, 0.9999998807907104}},
                .translation = {{4.76837158203125e-7, 17.137117385864258, 0.0000029802322387695312}}
            },
            {
                .children = {12},
                .rotation = {{0.9855465292930603, 0.1429160088300705, 0.06425860524177551, -0.0643734335899353}},
                .scale = {{1, 0.9999998211860657, 0.9999998211860657}},
                .translation = {{-0.0000015497207641601562, 19.279205322265625, 0.0000017881393432617188}}
            },
            {
                .children = {13},
                .rotation = {{0.9496142864227295, -0.039499398320913315, 0.3096625804901123, -0.027956483885645866}},
                .scale = {{1, 0.9999998211860657, 1}},
                .translation = {{-0.0000017881393432617188, 13.328019142150879, 4.077314853668213}}
            },
            {
                .children = {14},
                .rotation = {{1, 2.3283064365386963e-9, 2.6775524020195007e-9, -9.313225746154785e-10}},
                .translation = {{0.000004291534423828125, 44.09027099609375, 13.488086700439453}}
            },
            {
                .children = {15},
                .rotation = {{1, 2.3283064365386963e-9, 2.6775524020195007e-9, -9.313225746154785e-10}},
                .translation = {{0.000003337860107421875, 46.10726547241211, -1.4210854715202004e-14}}
            },
            {
                .rotation = {{1, 1.3877787807814457e-17, 1.3877787807814457e-17, 3.469446951953614e-18}},
                .translation = {{-8.881784197001252e-16, 46.107269287109375, 7.105427357601002e-15}}
            },
            {
                .children = {17},
                .rotation = {{0.33501380681991577, 0.6173083186149597, 0.36051011085510254, -0.6137823462486267}},
                .scale = {{0.9999998211860657, 0.9999998211860657, 1}},
                .translation = {{8.734130859375, 15.43148422241211, -0.42384397983551025}}
            },
            {
                .children = {18},
                .rotation = {{0.9749111533164978, 0.07651235908269882, -0.2030966430902481, -0.04945588484406471}},
                .scale = {{0.9999998807907104, 0.9999997615814209, 1}},
                .translation = {{-0.0000050067901611328125, 18.667898178100586, -0.00001430511474609375}}
            },
            {
                .children = {19},
                .rotation = {{0.6703649163246155, 0.09323903173208237, 0.0015500102890655398, 0.7361487150192261}},
                .scale = {{0.9999998211860657, 0.9999998211860657, 1}},
                .translation = {{-0.0000057220458984375, 43.49143981933594, 0.0000057220458984375}}
            },
            {
                .children = {20, 26, 32, 38, 44},
                .rotation = {{0.9698774218559265, 0.07580947130918503, -0.20728807151317596, -0.10306508094072342}},
                .scale = {{0.9999999403953552, 1, 0.9999998807907104}},
                .translation = {{0.0000057220458984375, 39.39338684082031, 0.00000667572021484375}}
            },
            {
                .children = {21},
                .rotation = {{0.9286826848983765, 0.35456833243370056, 0.060421645641326904, 0.09043771028518677}},
                .scale = {{0.9999997019767761, 0.9999998807907104, 0.9999998807907104}},
                .translation = {{-2.8492202758789062, 6.83029317855835, 2.382017135620117}}
            },
            {
                .children = {22},
                .rotation = {{0.9039925932884216, 0.1795351803302765, 0.045620307326316833, -0.38533535599708557}},
                .scale = {{1.0000001192092896, 1.000000238418579, 1.000000238418579}},
                .translation = {{-0.09836685657501221, 4.881401062011719, -4.653012275695801}}
            },
            {
                .children = {23},
                .rotation = {{0.992580771446228, -0.003961910028010607, 0.019130663946270943, -0.12000716477632523}},
                .scale = {{0.9999998211860657, 0.9999999403953552, 0.9999997019767761}},
                .translation = {{-1.0833683013916016, 2.967759370803833, -1.451798915863037}}
            },
            {
                .children = {24}, .rotation = {{1, 2.607703031287656e-8, 8.195638656616211e-8, -5.587935447692871e-8}},
                .scale = {{0.9999999403953552, 1.0000001192092896, 1}},
                .translation = {{0.7958478927612305, 7.086625099182129, -0.000008106231689453125}}
            },
            {
                .children = {25}, .rotation = {{1, 1.1175870895385742e-8, -1.862645149230957e-9, 1.30385160446167e-8}},
                .translation = {{0, 7.131178379058838, -0.00000667572021484375}}
            },
            {
                .rotation = {{1, -2.0816681711721685e-17, -2.7755575615628914e-17, 1.3877787807814457e-17}},
                .translation = {{0, 7.1311798095703125, 0}}
            },
            {
                .children = {27},
                .rotation = {{0.7045369744300842, 0.7031459808349609, 0.002292175544425845, -0.09595896303653717}},
                .scale = {{0.9999996423721313, 0.9999997019767761, 0.9999992847442627}},
                .translation = {{-5.988201141357422, 22.58428955078125, -0.28839111328125}}
            },
            {
                .children = {28},
                .rotation = {{0.6794496774673462, 0.7169684767723083, -0.006861151661723852, -0.15574732422828674}},
                .scale = {{1.0000001192092896, 1.000000238418579, 1.000000238418579}},
                .translation = {{0.1428375244140625, 4.872554779052734, -0.00000667572021484375}}
            },
            {
                .children = {29},
                .rotation = {{0.9164090752601624, 0.39087265729904175, 0.005840627010911703, -0.08590032905340195}},
                .scale = {{0.9999998211860657, 0.9999996423721313, 0.9999996423721313}},
                .translation = {{-0.03624081611633301, 4.478296756744385, -9.5367431640625e-7}}
            },
            {
                .children = {30},
                .rotation = {{1, -3.539025499321724e-7, -0.0000042710453271865845, 7.264314660915261e-8}},
                .scale = {{1, 1.000000238418579, 0.9999999403953552}},
                .translation = {{-0.10656917095184326, 4.155031204223633, 0.0000073909759521484375}}
            },
            {
                .children = {31},
                .rotation = {{1, 2.2351743567128324e-8, 1.6763806343078613e-8, -9.313227522511625e-9}},
                .scale = {{1, 0.9999999403953552, 1}},
                .translation = {{0.000007331371307373047, 4.156403064727783, 0.00001436471939086914}}
            },
            {
                .rotation = {{1, -4.163336342344337e-17, 2.7755575615628914e-17, -2.7755575615628914e-17}},
                .translation = {{0, 4.156402587890625, 2.842170943040401e-14}}
            },
            {
                .children = {33},
                .rotation = {{0.7055814266204834, 0.7023068070411682, -0.0078030782751739025, -0.09412293136119843}},
                .scale = {{0.9999996423721313, 0.9999998211860657, 0.9999998211860657}},
                .translation = {{-1.6778740882873535, 22.461694717407227, -1.0426316261291504}}
            },
            {
                .children = {34},
                .rotation = {{0.6765486001968384, 0.7155265808105469, -0.006759714335203171, -0.1739482581615448}},
                .scale = {{1.000000238418579, 1.0000001192092896, 0.9999997019767761}},
                .translation = {{0.25862598419189453, 5.591005325317383, -0.00001049041748046875}}
            },
            {
                .children = {35},
                .rotation = {{0.9168417453765869, 0.39009130001068115, 0.009701302275061607, -0.08447407931089401}},
                .scale = {{1, 0.9999999403953552, 1}},
                .translation = {{-0.11666560173034668, 5.142786026000977, -0.000017642974853515625}}
            },
            {
                .children = {36},
                .rotation = {{1, 5.327166263668914e-7, 0.000003568828333300189, -7.078052277620372e-8}},
                .scale = {{1.0000001192092896, 0.9999997019767761, 0.9999998211860657}},
                .translation = {{-0.14196258783340454, 4.800703048706055, -0.0000019073486328125}}
            },
            {
                .children = {37},
                .rotation = {{1, -1.1175870895385742e-8, -1.862645815364772e-9, 9.313225746154785e-9}},
                .scale = {{1, 1, 1.0000001192092896}},
                .translation = {{-0.0000011746305972337723, 4.802816867828369, 0.0000017350539565086365}}
            },
            {
                .rotation = {{1, 3.851859888774472e-34, 5.551115123125783e-17, -6.938893903907228e-18}},
                .translation = {{0, 4.802816867828369, 0}}
            },
            {
                .children = {39},
                .rotation = {{0.7049331068992615, 0.7005804181098938, -0.012660044245421886, -0.10998228192329407}},
                .scale = {{0.9999995231628418, 1, 0.9999998807907104}},
                .translation = {{2.3555855751037598, 20.918235778808594, -0.8685369491577148}}
            },
            {
                .children = {40},
                .rotation = {{0.6788572072982788, 0.7107225656509399, -0.006459805648773909, -0.1843491494655609}},
                .scale = {{1.0000001192092896, 1.0000004768371582, 1}},
                .translation = {{0.1590561866760254, 5.1895012855529785, -0.000002384185791015625}}
            },
            {
                .children = {41},
                .rotation = {{0.9160821437835693, 0.38749316334724426, 0.004888961091637611, -0.10304706543684006}},
                .scale = {{0.9999999403953552, 0.9999998211860657, 0.9999998211860657}},
                .translation = {{-0.03585004806518555, 4.82564640045166, 0.000023126602172851562}}
            },
            {
                .children = {42}, .rotation = {{1, 3.725289587919178e-8, 7.078050145992165e-8, -1.862645149230957e-8}},
                .scale = {{1, 1.0000001192092896, 1.0000003576278687}},
                .translation = {{-0.12318873405456543, 4.298894882202148, 0.0000032782554626464844}}
            },
            {
                .children = {43},
                .rotation = {{1, -3.725290298461914e-9, 5.606562467619369e-7, -1.0617078061159191e-7}},
                .scale = {{0.9999998807907104, 1, 0.9999999403953552}},
                .translation = {{-0.000021144747734069824, 4.300668716430664, -0.000001043081283569336}}
            },
            {
                .rotation = {{1, 7.703719777548943e-34, 2.7755575615628914e-17, -2.7755575615628914e-17}},
                .translation = {{2.842170943040401e-14, 4.300668239593506, 0}}
            },
            {
                .children = {45},
                .rotation = {{0.7041096687316895, 0.6880173683166504, -0.0010389413218945265, -0.1756717413663864}},
                .scale = {{0.9999998211860657, 0.999999463558197, 0.9999995827674866}},
                .translation = {{5.310442924499512, 18.544679641723633, -0.7035939693450928}}
            },
            {
                .children = {46},
                .rotation = {{0.6831808090209961, 0.7015658617019653, 0.027359716594219208, -0.20080038905143738}},
                .scale = {{1.0000001192092896, 1.000000238418579, 0.9999997615814209}},
                .translation = {{-0.04586505889892578, 4.710577964782715, 4.76837158203125e-7}}
            },
            {
                .children = {47},
                .rotation = {{0.9173334836959839, 0.3784691393375397, -0.0023873932659626007, -0.12350989133119583}},
                .scale = {{0.9999999403953552, 1.0000001192092896, 1.000000238418579}},
                .translation = {{0.01806187629699707, 3.6776230335235596, 0.000006556510925292969}}
            },
            {
                .children = {48}, .rotation = {{1, 1.415610029198433e-7, 3.278254325778107e-7, -2.179294114057484e-7}},
                .scale = {{1.0000003576278687, 1.000000238418579, 1.000000238418579}},
                .translation = {{0.027845710515975952, 3.382868528366089, -0.00000286102294921875}}
            },
            {
                .children = {49},
                .rotation = {{1, 1.862645326866641e-8, -3.725290298461914e-9, -1.8626449271863521e-9}},
                .scale = {{1, 0.9999999403953552, 1}},
                .translation = {{0.000006020069122314453, 3.3829493522644043, 0.0000021457672119140625}}
            },
            {
                .rotation = {{1, 3.851859888774472e-34, 5.551115123125783e-17, -6.938893903907228e-18}},
                .translation = {{0, 3.382948875427246, -2.842170943040401e-14}}
            },
            {
                .children = {51},
                .rotation = {{0.33148375153541565, 0.611207127571106, -0.3740321397781372, 0.6137135624885559}},
                .scale = {{0.9999997019767761, 0.9999996423721313, 0.9999997019767761}},
                .translation = {{-8.73413372039795, 15.439642906188965, -0.4923233985900879}}
            },
            {
                .children = {52},
                .rotation = {{0.8105257153511047, 0.24095240235328674, 0.06982076913118362, -0.5292589664459229}},
                .scale = {{1, 1.0000001192092896, 0.9999998807907104}},
                .translation = {{-0.000003814697265625, 18.66789436340332, 0.0000209808349609375}}
            },
            {
                .children = {53},
                .rotation = {{0.57444828748703, 0.10517097264528275, -0.0012382488930597901, -0.8117552995681763}},
                .scale = {{1, 0.9999997615814209, 0.9999999403953552}},
                .translation = {{-0.000012874603271484375, 43.49249267578125, 0.00002002716064453125}}
            },
            {
                .children = {54, 60, 66, 72, 78},
                .rotation = {{0.9830542802810669, -0.06675019860267639, 0.17028279602527618, 0.01234945748001337}},
                .scale = {{0.9999997019767761, 0.9999998807907104, 0.9999996423721313}},
                .translation = {{-0.0000095367431640625, 39.33510971069336, 0.00001621246337890625}}
            },
            {
                .children = {55},
                .rotation = {{0.9205004572868347, 0.276485413312912, -0.2321697175502777, -0.14943872392177582}},
                .scale = {{0.9999998807907104, 0.9999998807907104, 0.9999998211860657}},
                .translation = {{3.312925338745117, 7.211411476135254, 2.960620403289795}}
            },
            {
                .children = {56},
                .rotation = {{0.7994732856750488, 0.31196877360343933, -0.1324247568845749, 0.4959653317928314}},
                .scale = {{1, 0.9999997615814209, 1.000000238418579}},
                .translation = {{0.8498373031616211, 10.761335372924805, -6.215497016906738}}
            },
            {
                .children = {57},
                .rotation = {{0.9902334809303284, 0.11031853407621384, 0.008911092765629292, 0.08478246629238129}},
                .scale = {{0.9999999403953552, 0.9999993443489075, 0.9999997615814209}},
                .translation = {{0.5567317008972168, 6.821746349334717, -0.000017642974853515625}}
            },
            {
                .children = {58},
                .rotation = {{1, 3.0687078833580017e-7, -0.0000011175870895385742, -2.3003664750831376e-7}},
                .scale = {{1.000000238418579, 1, 0.9999999403953552}},
                .translation = {{-1.2000503540039062, 5.913128852844238, 0}}
            },
            {
                .children = {59},
                .rotation = {{1, -1.4901161193847656e-8, 0.0000012051314115524292, 8.675269214108994e-7}},
                .scale = {{1.0000001192092896, 1, 1}},
                .translation = {{-0.0000019073486328125, 6.033651351928711, -0.0000040531158447265625}}
            },
            {
                .rotation = {{1, 2.7755575615628914e-17, -1.3877787807814457e-17, 2.7755575615628914e-17}},
                .translation = {{-7.105427357601002e-15, 6.033647537231445, 0}}
            },
            {
                .children = {61},
                .rotation = {{0.6859766840934753, 0.7204020619392395, -0.0076073273085057735, 0.10197529196739197}},
                .scale = {{0.9999999403953552, 0.9999997019767761, 1.0000001192092896}},
                .translation = {{6.088129997253418, 22.46143913269043, -0.29377317428588867}}
            },
            {
                .children = {62},
                .rotation = {{0.7463116645812988, 0.6481469869613647, 0.009311649948358536, 0.15112145245075226}},
                .scale = {{0.9999998807907104, 1, 0.9999997019767761}},
                .translation = {{-0.1698007583618164, 4.735726356506348, -0.0000035762786865234375}}
            },
            {
                .children = {63},
                .rotation = {{0.9337901473045349, 0.34653425216674805, -0.005307519342750311, 0.08900485187768936}},
                .scale = {{1.0000001192092896, 1.0000001192092896, 0.9999998807907104}},
                .translation = {{0.013940811157226562, 4.564804553985596, -9.5367431640625e-7}}
            },
            {
                .children = {64}, .rotation = {{1, -8.19563723553074e-8, 7.227060905279359e-7, 3.259628655882807e-8}},
                .scale = {{1.0000001192092896, 1.0000001192092896, 1.0000003576278687}},
                .translation = {{0.15583395957946777, 4.183650016784668, -0.0000050067901611328125}}
            },
            {
                .children = {65}, .rotation = {{1, 1.4901161193847656e-8, 1.5273690223693848e-7, -2.07684919928397e-7}},
                .scale = {{1.0000001192092896, 1, 1}},
                .translation = {{0.0000035762786865234375, 4.186527252197266, 0.000026941299438476562}}
            },
            {
                .rotation = {{1, 1.1555579666323415e-33, 1.3877787807814457e-17, -8.326672684688674e-17}},
                .translation = {{1.7763568394002505e-15, 4.1865386962890625, 2.842170943040401e-14}}
            },
            {
                .children = {67},
                .rotation = {{0.6868546605110168, 0.7199157476425171, 0.0003641488729044795, 0.09975901246070862}},
                .scale = {{0.9999998807907104, 0.9999999403953552, 1}},
                .translation = {{2.003570556640625, 21.854610443115234, -0.9734129905700684}}
            },
            {
                .children = {68},
                .rotation = {{0.743750810623169, 0.6473201513290405, 0.007551863789558411, 0.16659623384475708}},
                .scale = {{0.9999998807907104, 1, 0.9999998211860657}},
                .translation = {{-0.29830360412597656, 5.897706031799316, 0.0000022649765014648438}}
            },
            {
                .children = {69},
                .rotation = {{0.9343750476837158, 0.346140056848526, -0.008400768041610718, 0.08402255177497864}},
                .scale = {{1.0000003576278687, 0.9999998807907104, 1.0000001192092896}},
                .translation = {{0.1013031005859375, 5.281160831451416, 0.0000019073486328125}}
            },
            {
                .children = {70},
                .rotation = {{1, -7.115304470062256e-7, 0.000003114343144261511, -9.96515154838562e-8}},
                .scale = {{0.9999998211860657, 1.0000001192092896, 0.9999998807907104}},
                .translation = {{0.1970052719116211, 5.005071640014648, -0.0000400543212890625}}
            },
            {
                .children = {71},
                .rotation = {{1, -7.450580596923828e-9, 6.938893903907228e-17, -5.587935447692871e-9}},
                .translation = {{0.0000019073486328125, 5.008918762207031, 0.000011920928955078125}}
            },
            {
                .rotation = {{1, 5.551115123125783e-17, 1.3877787807814457e-17, 2.7755575615628914e-17}},
                .translation = {{-1.7763568394002505e-15, 5.008910655975342, 0}}
            },
            {
                .children = {73},
                .rotation = {{0.6862017512321472, 0.717044472694397, 0.007080353330820799, 0.12216479331254959}},
                .scale = {{0.9999998211860657, 0.9999998211860657, 0.9999998211860657}},
                .translation = {{-2.468820571899414, 21.199771881103516, -0.8859109878540039}}
            },
            {
                .children = {74},
                .rotation = {{0.7453559637069702, 0.6405521631240845, 0.00394390057772398, 0.18472100794315338}},
                .scale = {{1.000000238418579, 1, 1.000000238418579}},
                .translation = {{-0.15084075927734375, 5.1460466384887695, -0.0000030994415283203125}}
            },
            {
                .children = {75},
                .rotation = {{0.9347752332687378, 0.3425270915031433, -0.005357686430215836, 0.09403036534786224}},
                .scale = {{0.9999998807907104, 0.9999998807907104, 0.9999999403953552}},
                .translation = {{0.0595245361328125, 4.524462699890137, 0.000003814697265625}}
            },
            {
                .children = {76},
                .rotation = {{1, -4.246829803378205e-7, 0.0000029373904908425175, -9.872017869838601e-8}},
                .scale = {{1.0000001192092896, 1.000000238418579, 1.0000003576278687}},
                .translation = {{0.09131622314453125, 4.175976753234863, 0.0000095367431640625}}
            },
            {
                .children = {77}, .rotation = {{1, 3.725290298461914e-9, 3.725290298461914e-9, 5.587935447692871e-9}},
                .translation = {{-0.00000286102294921875, 4.176959991455078, -0.00001239776611328125}}
            },
            {
                .rotation = {{1, -1.3877787807814457e-17, 2.7755575615628914e-17, 3.851859888774472e-34}},
                .translation = {{0, 4.1769609451293945, -2.842170943040401e-14}}
            },
            {
                .children = {79},
                .rotation = {{0.6841571927070618, 0.7111281156539917, 0.02519179880619049, 0.1599726378917694}},
                .scale = {{0.9999999403953552, 0.9999999403953552, 0.9999999403953552}},
                .translation = {{-5.622869491577148, 18.709766387939453, -0.9215183258056641}}
            },
            {
                .children = {80},
                .rotation = {{0.7485918998718262, 0.6245567798614502, 0.00029693410033360124, 0.222573384642601}},
                .scale = {{1.0000003576278687, 0.9999999403953552, 0.9999999403953552}},
                .translation = {{-0.013568878173828125, 4.6107282638549805, 0.000016570091247558594}}
            },
            {
                .children = {81},
                .rotation = {{0.9351544380187988, 0.3339337706565857, -0.000601131992880255, 0.11821199953556061}},
                .scale = {{0.9999998211860657, 0.9999998807907104, 0.9999998807907104}},
                .translation = {{0.00634765625, 3.6694459915161133, 0.0000095367431640625}}
            },
            {
                .children = {82}, .rotation = {{1, -4.619358833224396e-7, 2.738087800935318e-7, -3.958120231573048e-8}},
                .scale = {{1.0000001192092896, 1.000000238418579, 1.000000238418579}},
                .translation = {{0.007198333740234375, 3.452136993408203, -0.0000095367431640625}}
            },
            {
                .children = {83},
                .rotation = {{1, 0.0000019855799564538756, -0.0000018179418930230895, 5.406328114077041e-7}},
                .scale = {{0.9999999403953552, 0.9999999403953552, 0.9999998807907104}},
                .translation = {{-0.000009059906005859375, 3.4521188735961914, 0.0000171661376953125}}
            },
            {
                .rotation = {{1, -1.3877787807814457e-17, -1.3877787807814457e-17, 2.7755575615628914e-17}},
                .translation = {{0, 3.4521243572235107, 2.842170943040401e-14}}
            },
            {
                .children = {85},
                .rotation = {{-0.17547039687633514, 0.216592475771904, 0.05013863369822502, 0.9590536952018738}},
                .scale = {{1.0000207424163818, 1.0000007152557373, 1.0000054836273193}},
                .translation = {{14.929533958435059, -7.085418224334717, -0.20067596435546875}}
            },
            {
                .children = {86},
                .rotation = {{0.9435425996780396, -0.31911084055900574, 0.07953031361103058, -0.03963040933012962}},
                .scale = {{0.999999463558197, 0.9999999403953552, 0.9999998211860657}},
                .translation = {{-0.00000286102294921875, 74.70596313476562, -0.0000019073486328125}}
            },
            {
                .children = {87},
                .rotation = {{0.8934839367866516, 0.4473780691623688, -0.002109020948410034, 0.03918025642633438}},
                .scale = {{1.0000038146972656, 0.9999974370002747, 0.9999991655349731}},
                .translation = {{-0.0000021457672119140625, 76.67253875732422, 0.00000476837158203125}}
            },
            {
                .children = {88},
                .rotation = {{0.9475346207618713, 0.30893561244010925, -0.08205144852399826, 0.0021738559007644653}},
                .scale = {{1.0000004768371582, 0.9999973177909851, 1.0000022649765015}},
                .translation = {{-0.0000021457672119140625, 27.122655868530273, -0.0000019073486328125}}
            },
            {
                .children = {89},
                .rotation = {{1, 4.8428770327291204e-8, 0.0000017434358596801758, 4.330650682504711e-8}},
                .scale = {{0.9999997615814209, 0.9999999403953552, 1.000000238418579}},
                .translation = {{0.0000028908252716064453, 10.348443984985352, 5.960464477539063e-8}}
            },
            {
                .children = {90},
                .rotation = {{1, -1.994929019523123e-17, -9.313225746154785e-9, 5.4482367062291814e-8}},
                .scale = {{1, 1.0000001192092896, 1}},
                .translation = {{-0.000004605921276379377, 10.348499298095703, 4.019821062684059e-7}}
            },
            {.translation = {{0, 10.348553657531738, 0}}},
            {
                .children = {92},
                .rotation = {{0.11407258361577988, -0.10778479278087616, -0.03620857372879982, 0.9869441986083984}},
                .scale = {{1.0000227689743042, 1, 1.000004768371582}},
                .translation = {{-14.929535865783691, -7.085418224334717, -0.04557609558105469}}
            },
            {
                .children = {93},
                .rotation = {{0.9737575650215149, -0.21057084202766418, -0.08154168725013733, 0.028408704325556755}},
                .scale = {{0.9999997615814209, 1.000000238418579, 1}},
                .translation = {{-5.960464477539062e-7, 74.71305084228516, 2.384185791015625e-7}}
            },
            {
                .children = {94},
                .rotation = {{0.9085890054702759, 0.4109816551208496, 0.07399804145097733, -0.00918839406222105}},
                .scale = {{1.0000028610229492, 0.9999972581863403, 1}},
                .translation = {{0.000001430511474609375, 76.73368835449219, 0.0000019073486328125}}
            },
            {
                .children = {95},
                .rotation = {{0.8976038098335266, 0.4323638379573822, 0.08525895327329636, -0.009991203434765339}},
                .scale = {{1.0000003576278687, 0.9999967813491821, 1.0000019073486328}},
                .translation = {{-0.00000858306884765625, 27.95172119140625, -4.76837158203125e-7}}
            },
            {
                .children = {96},
                .rotation = {{1, 3.722743358025582e-8, -0.00000200513727577345, 1.1500377183892851e-7}},
                .scale = {{1.0000001192092896, 1.0000001192092896, 1.0000001192092896}},
                .translation = {{0.00000522565096616745, 10.435144424438477, -5.960464477539063e-8}}
            },
            {
                .children = {97}, .rotation = {{1, 2.533852239139378e-9, -4.674075171351433e-8, 3.2378011383116245e-8}},
                .scale = {{1, 0.9999999403953552, 1}},
                .translation = {{0.0000064449559431523085, 10.435200691223145, -7.82310962677002e-8}}
            },
            {
                .rotation = {{1, 7.569086416664428e-18, -2.6260543797369903e-35, -3.469446951953614e-18}},
                .translation = {{0, 10.435270309448242, 0}}
            },
            {
                .matrix = {
                    {
                        100.00000000000001, 1.043081138830469e-7, -0.0000026151537725263856, 0,
                        -0.0000026151537723030243, -2.1420311851115369e-7, -100.00000000000269, 0,
                        -1.0430811948479056e-7, 100.00000000000269, -2.1420311578333599e-7, 0,
                        -0.0000020600855350494385, 2.505266820662655e-7, -0.000003212198698143709, 1
                    }
                }
            },
            {.mesh = 0},
            {
                .matrix = {
                    {
                        99.99999999999999, -0.000002041458946391711, -8.419157126746003e-7, 0, -8.419157271050666e-7,
                        -7.068691176878003e-7, -100.00000000000279, 0, 0.0000020414589404405253, 100.00000000000276,
                        -7.068691348751642e-7, 0, -0.0000060945744735363405, -0.000015008262352012025,
                        -0.000003331410473705887, 1
                    }
                }
            },
            {.mesh = 1},
            {
                .matrix = {
                    {
                        100, 9.38773145916992e-7, -6.407498920767534e-7, 0, -6.407498929161261e-7, 8.940985740371206e-8,
                        -100.00000000000263, 0, -9.387731453441233e-7, 100.00000000000266, 8.940986341890001e-8, 0,
                        -8.195638656616211e-8, 2.5052665364455606e-7, -0.0000029737801190421465, 1
                    }
                }
            },
            {.mesh = 2},
            {
                .matrix = {
                    {
                        100, -1.490116134095134e-8, -7.078051591354897e-7, 0, -7.078051591631254e-7,
                        -1.8533212119385482e-7, -100.0000000000027, 0, 1.4901160029161422e-8, 100.00000000000269,
                        -1.8533212129932598e-7, 0, -1.5273690223693848e-7, 2.505265968011372e-7,
                        -0.000002735361539940584, 1
                    }
                }
            },
            {.mesh = 3},
            {
                .matrix = {
                    {
                        100, -1.490116134095134e-8, -7.078051591354897e-7, 0, -7.078051591631254e-7,
                        -1.8533212119385482e-7, -100.0000000000027, 0, 1.4901160029161422e-8, 100.00000000000269,
                        -1.8533212129932598e-7, 0, -1.5273690223693848e-7, 2.505265968011372e-7,
                        -0.000002735361539940584, 1
                    }
                }
            },
            {.mesh = 4},
            {
                .rotation = {{0.7071067094802856, -0.7071068286895752, 8.771813497787662e-9, 4.609812087608134e-9}},
                .scale = {{100, 100, 100}},
                .translation = {{-0.0000020600855350494385, -1.6950069436916237e-7, -0.0000031739471069158753}}
            },
            {
                .rotation = {{0.7071067094802856, -0.7071068286895752, 1.0220612800537765e-8, -3.002962323250813e-9}},
                .scale = {{100, 100, 100}},
                .translation = {{-0.0000060945744735363405, -0.000017334705262328498, -0.0000033229589462280273}}
            },
            {
                .rotation = {{0.7071067094802856, -0.7071068286895752, -1.211722167049345e-9, 8.429368314466501e-10}},
                .scale = {{100, 100, 100}},
                .translation = {{-8.195638656616211e-8, -0.0000049388036131858826, -0.0000030100338790362002}}
            },
            {
                .rotation = {{0.7071067094802856, -0.7071068286895752, 2.4629569495004944e-9, -2.5551527560452314e-9}},
                .scale = {{100, 100, 100}},
                .translation = {{-1.5273690223693848e-7, -1.7043203115463257e-7, -0.0000026971101760864258}}
            },
            {
                .rotation = {{0.7071067094802856, -0.7071068286895752, 2.4629569495004944e-9, -2.5551527560452314e-9}},
                .scale = {{100, 100, 100}},
                .translation = {{-1.5273690223693848e-7, -1.7043203115463257e-7, -0.0000026971101760864258}}
            }
        },
        .scene = 0, .scenes = {{.nodes = {0}}}
    };

    for (auto& buffer : model.buffers)
    {
        if (!buffer.data.empty())
            continue;

        buffer.data = readFileToVector(buffer.uri, buffer.byteLength);
    }

    ShaderProgramVariants programVariants(RESOURCE_PATH "shaders/default.vert", RESOURCE_PATH "shaders/default.frag");
    programVariants.enableVariant(ShaderHasNone);
    ShaderProgram program = programVariants.getProgram(ShaderHasNone); // ONLY USE ONE SHADER

    auto vertexArray = VertexArray();

    std::unordered_map<size_t, GLuint> buffers;
    prepare(model, vertexArray, buffers);

    CameraController c({0, 1, 0}, 5);

    vertexArray.bind();
    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    engine.run([&](Engine& engine)
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Will be automatized
        c.update(engine.getWindow().getCurrentControls(), camera, engine.frameInfo().deltaTime.count());

        const auto pvMat = camera.projectionMatrix() * camera.computeViewMatrix();
        program.use();
        program.setMat4("projectionView", pvMat);

        for (const auto nodeIndex : model.scenes[model.scene].nodes)
            renderNode(model, nodeIndex, program, buffers, glm::scale(glm::identity<glm::mat4>(), glm::vec3(100)));
    });

    return {};
}

auto main() -> int
{
    auto e_result = start();
    if (!e_result)
    {
        std::cerr << "Error: " << e_result.error() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
