#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "HumanGLConfig.h"
#include "Camera.h"
#include "Engine.h"
#include "Window.h"
#include "WindowContext.h"
#include "MicroGLTF/Model.h"
#include "OpenGL/Shader.h"
#include "OpenGL/ShaderProgramVariants.h"
#include "OpenGL/VertexArray.h"
#include "Scripts/CameraController.h"

constexpr GLfloat vertex[] = {
    -0.5, -0.5, 0.5, //  0 : Front-bottom-left
    0.5, -0.5, 0.5, //   1 : Front-bottom-right
    0.5, 0.5, 0.5, //    2 : Front-top-right
    -0.5, 0.5, 0.5, //   3 : Front-top-left
    -0.5, -0.5, -0.5, // 4 : Back-bottom-left
    0.5, -0.5, -0.5, //  5 : Back-bottom-right
    0.5, 0.5, -0.5, //   6 : Back-top-right
    -0.5, 0.5, -0.5, //  7 : Back-top-left
};
constexpr GLuint indices[] = {
    // Triangle1 // Triangle2
    0, 1, 2, 2, 3, 0, // Front
    6, 5, 4, 4, 7, 6, // Back
    4, 0, 3, 3, 7, 4, // Left
    1, 5, 6, 6, 2, 1, // Right
    3, 2, 6, 6, 7, 3, // Up
    4, 5, 1, 1, 0, 4, // Down
};
constexpr GLfloat colors[] = {
    1.0f, 0.0f, 0.0f, // Red 0
    0.0f, 1.0f, 0.0f, // Green 1
    0.0f, 0.0f, 1.0f, // Blue 2
    1.0f, 1.0f, 0.0f, // Yellow 3
    1.0f, 0.0f, 1.0f, // Magenta 4
    0.0f, 1.0f, 1.0f, // Cyan 5
    1.0f, 1.0f, 1.0f, // White 6
    0.0f, 0.0f, 0.0f //  Black 7
};

constexpr size_t indicesViewSize = sizeof(indices);
constexpr size_t vertexViewSize = sizeof(vertex);
constexpr size_t colorsViewSize = sizeof(colors);
constexpr size_t indicesCount = indicesViewSize / sizeof(*indices);
constexpr size_t vertexCount = vertexViewSize / sizeof(*vertex) / 3;
constexpr size_t colorsCount = vertexViewSize / sizeof(*colors) / 3;
constexpr size_t bufferSize = indicesViewSize + vertexViewSize + colorsViewSize;

void* bufferOffset(const size_t offset)
{
    return reinterpret_cast<void*>(offset);
}

auto renderMesh(const microgltf::Model& model, const int meshIndex, ShaderProgram& program,
                const std::unordered_map<size_t, GLuint>& buffers, glm::mat4 transform) -> void
{
    const microgltf::Mesh& mesh = model.meshes[meshIndex];

    for (const auto& primitive : mesh.primitives)
    {
        for (const auto& [attribute, accessorIndex] : primitive.attributes)
        {
            const microgltf::Accessor& accessor = model.accessors[accessorIndex];

            glBindBuffer(GL_ARRAY_BUFFER, buffers.at(accessor.bufferView));

            const int attributeLocation = program.getAttributeLocation(attribute);
            if (attributeLocation != -1)
            {
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
                const std::unordered_map<size_t, GLuint>& buffers,
                glm::mat4 transform = glm::identity<glm::mat4>()) -> void
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

    // std::vector<GLubyte> cubeBufferData;
    // cubeBufferData.reserve(bufferSize);
    // cubeBufferData.insert(cubeBufferData.end(), reinterpret_cast<const GLubyte*>(indices),
    //                       reinterpret_cast<const GLubyte*>(indices) + indicesViewSize);
    // cubeBufferData.insert(cubeBufferData.end(), reinterpret_cast<const GLubyte*>(vertex),
    //                       reinterpret_cast<const GLubyte*>(vertex) + vertexViewSize);
    // cubeBufferData.insert(cubeBufferData.end(), reinterpret_cast<const GLubyte*>(colors),
    //                       reinterpret_cast<const GLubyte*>(colors) + colorsViewSize);

    // microgltf::Model model = {
    //     .scene = 0,
    //     .scenes = {
    //         {
    //             .nodes = {0},
    //         }
    //     },
    //     .nodes = {
    //         {
    //             .children = {1, 2, 3, 5, 7, 9},
    //             .name = "",
    //         },
    //         {
    //             .mesh = 0,
    //             .scale = {{1, 1.5, 0.5}},
    //             .translation = {{0, 0.75, 0}},
    //             .name = "body",
    //         },
    //         {
    //             .mesh = 0,
    //             .scale = {{1, 1, 1}},
    //             .translation = {{0, 2.25, 0}},
    //             .name = "head",
    //         },
    //         {
    //             .children = {4},
    //             .translation = {{-0.75, 1.5, 0}},
    //             .name = "arm_left_pivot",
    //         },
    //         {
    //             .mesh = 0,
    //             .scale = {{0.5, 1.5, 0.5}},
    //             .translation = {{-0.25, -0.75, 0}},
    //             .name = "arm_left",
    //         },
    //         {
    //             .children = {6},
    //             .translation = {{0.75, 1.5, 0}},
    //             .name = "arm_right_pivot",
    //         },
    //         {
    //             .mesh = 0,
    //             .scale = {{0.5, 1.5, 0.5}},
    //             .translation = {{0.25, -0.75, 0}},
    //             .name = "arm_right",
    //         },
    //         {
    //             .children = {8},
    //             .translation = {{-0.25, -0.75, 0}},
    //             .name = "leg_left_pivot",
    //         },
    //         {
    //             .mesh = 0,
    //             .scale = {{0.5, 1.5, 0.5}},
    //             .translation = {{-0.25, -0.75, 0}},
    //             .name = "leg_left",
    //         },
    //         {
    //             .children = {10},
    //             .translation = {{0.25, -0.75, 0}},
    //             .name = "leg_right_pivot",
    //         },
    //         {
    //             .mesh = 0,
    //             .scale = {{0.5, 1.5, 0.5}},
    //             .translation = {{0.25, -0.75, 0}},
    //             .name = "leg_right",
    //         },
    //     },
    //     .meshes = {
    //         {
    //             .primitives = {
    //                 {
    //                     .indices = 0,
    //                     .attributes = {
    //                         {"POSITION", 1},
    //                         {"COLORS", 2},
    //                     },
    //                     .mode = GL_TRIANGLES,
    //                 },
    //             },
    //         },
    //     },
    //     .accessors = {
    //         {
    //             .bufferView = 0,
    //             .byteOffset = 0,
    //             .type = microgltf::Scalar,
    //             .componentType = GL_UNSIGNED_INT,
    //             .count = indicesCount,
    //         },
    //         {
    //             .bufferView = 1,
    //             .byteOffset = 0,
    //             .type = microgltf::Vec3,
    //             .componentType = GL_FLOAT,
    //             .count = vertexCount,
    //         },
    //         {
    //             .bufferView = 2,
    //             .byteOffset = 0,
    //             .type = microgltf::Vec3,
    //             .componentType = GL_FLOAT,
    //             .count = colorsCount,
    //         },
    //     },
    //     .bufferViews = {
    //         {
    //             .buffer = 0,
    //             .byteOffset = 0,
    //             .byteLength = indicesViewSize,
    //             .byteStride = 0,
    //             .target = GL_ELEMENT_ARRAY_BUFFER,
    //         },
    //         {
    //             .buffer = 0,
    //             .byteOffset = indicesViewSize,
    //             .byteLength = vertexViewSize,
    //             .byteStride = sizeof(*vertex) * 3,
    //             .target = GL_ARRAY_BUFFER,
    //         },
    //         {
    //             .buffer = 0,
    //             .byteOffset = indicesViewSize + vertexViewSize,
    //             .byteLength = colorsViewSize,
    //             .byteStride = sizeof(*colors) * 3,
    //             .target = GL_ARRAY_BUFFER,
    //         },
    //     },
    //     .buffers = {
    //         {
    //             .data = std::move(cubeBufferData),
    //         },
    //     },
    // };

    microgltf::Model model = {
        .accessors = {
            {.bufferView = 3, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 288, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 576, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 864, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 192, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 144, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 1152, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 1440, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 384, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 288, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 1728, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 2016, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 576, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 432, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 2304, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 2592, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 768, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 576, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 2880, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 3168, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 960, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 720, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 3456, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 3744, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 1152, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 864, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 4032, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 4320, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 1344, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 1008, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 4608, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 4896, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 1536, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 1152, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 5184, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 5472, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 1728, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 1296, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 5760, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 6048, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 1920, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 1440, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 3, .byteOffset = 6336, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 3, .byteOffset = 6624, .componentType = 5126, .count = 24, .type = microgltf::Vec3},
            {.bufferView = 2, .byteOffset = 2112, .componentType = 5126, .count = 24, .type = microgltf::Vec2},
            {.bufferView = 1, .byteOffset = 1584, .componentType = 5125, .count = 36, .type = microgltf::Scalar},
            {.bufferView = 5, .componentType = 5126, .count = 17, .type = microgltf::Mat4},
            {.bufferView = 0, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 192, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 384, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 384, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 768, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 576, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 1152, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 768, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 1536, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 960, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 1920, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 1152, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 2304, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 1344, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 2688, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 1536, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 3072, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 1728, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 3456, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 1920, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 3840, .componentType = 5126, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 0, .byteOffset = 2112, .componentType = 5123, .count = 24, .type = microgltf::Vec4},
            {.bufferView = 4, .byteOffset = 4224, .componentType = 5126, .count = 24, .type = microgltf::Vec4}
        },
        .bufferViews = {
            {.buffer = 0, .byteLength = 2304, .byteStride = 8, .target = 34962},
            {.buffer = 0, .byteLength = 1728, .byteOffset = 2304, .target = 34963},
            {
                .buffer = 0, .byteLength = 2304, .byteOffset = 4032, .byteStride = 8,
                .target = 34962
            },
            {
                .buffer = 0, .byteLength = 6912, .byteOffset = 6336, .byteStride = 12,
                .target = 34962
            },
            {
                .buffer = 0, .byteLength = 4608, .byteOffset = 13248, .byteStride = 16,
                .target = 34962
            },
            {.buffer = 0, .byteLength = 1088, .byteOffset = 17856}
        },
        .buffers = {{.byteLength = 18944, .uri = RESOURCE_PATH "models/minecraft_player/scene.bin"}},
        .meshes = {
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 9}, {"NORMAL", 1}, {"POSITION", 0}, {"TEXCOORD_0", 2}, {"WEIGHTS_0", 0}
                        },
                        .indices = 3, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 1}, {"NORMAL", 5}, {"POSITION", 4}, {"TEXCOORD_0", 6}, {"WEIGHTS_0", 2}
                        },
                        .indices = 7, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 3}, {"NORMAL", 9}, {"POSITION", 8}, {"TEXCOORD_0", 0}, {"WEIGHTS_0", 4}
                        },
                        .indices = 11, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 5}, {"NORMAL", 3}, {"POSITION", 2}, {"TEXCOORD_0", 4}, {"WEIGHTS_0", 6}
                        },
                        .indices = 15, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 7}, {"NORMAL", 7}, {"POSITION", 6}, {"TEXCOORD_0", 8}, {"WEIGHTS_0", 8}
                        },
                        .indices = 19, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 9}, {"NORMAL", 1}, {"POSITION", 0}, {"TEXCOORD_0", 2}, {"WEIGHTS_0", 0}
                        },
                        .indices = 23, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 1}, {"NORMAL", 5}, {"POSITION", 4}, {"TEXCOORD_0", 6}, {"WEIGHTS_0", 2}
                        },
                        .indices = 27, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 3}, {"NORMAL", 9}, {"POSITION", 8}, {"TEXCOORD_0", 0}, {"WEIGHTS_0", 4}
                        },
                        .indices = 31, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 5}, {"NORMAL", 3}, {"POSITION", 2}, {"TEXCOORD_0", 4}, {"WEIGHTS_0", 6}
                        },
                        .indices = 35, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 7}, {"NORMAL", 7}, {"POSITION", 6}, {"TEXCOORD_0", 8}, {"WEIGHTS_0", 8}
                        },
                        .indices = 39, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 9}, {"NORMAL", 1}, {"POSITION", 0}, {"TEXCOORD_0", 2}, {"WEIGHTS_0", 0}
                        },
                        .indices = 43, .mode = 4
                    }
                }
            },
            {
                .primitives = {
                    {
                        .attributes = {
                            {"JOINTS_0", 1}, {"NORMAL", 5}, {"POSITION", 4}, {"TEXCOORD_0", 6}, {"WEIGHTS_0", 2}
                        },
                        .indices = 47, .mode = 4
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
            {.children = {3}},
            {
                .children = {4},
                .matrix = {
                    {
                        100, 0, 0, 0, 0, -0.00001629206793918314, -99.99999999999868, 0, 0, 99.99999999999868,
                        -0.00001629206793918314, 0, 0, -370, 0, 1
                    }
                }
            },
            {
                .children = {
                    5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28
                }
            },
            {.children = {30}},
            {
                .matrix = {
                    {
                        79.99999523162842, 0, 0, 0, 0, 4.898586904610863e-15, -79.99999523162842, 0, 0,
                        79.99999523162842, 4.898586904610863e-15, 0, 0, 199.99994277954102, 0, 1
                    }
                }
            },
            {.mesh = 0},
            {
                .matrix = {
                    {
                        89.99999761581421, 0, 0, 0, 0, 5.510910450173815e-15, -89.99999761581421, 0, 0,
                        89.99999761581421, 5.510910450173815e-15, 0, 0, 199.99994277954102, 0, 1
                    }
                }
            },
            {.mesh = 1},
            {
                .matrix = {
                    {
                        39.99999761581421, 0, 0, 0, 0, 4.898586904610863e-15, -79.99999523162842, 0, 0,
                        120.00000476837158, 7.347881086862668e-15, 0, 0, -0.0000095367431640625, 0, 1
                    }
                }
            },
            {.mesh = 2},
            {
                .matrix = {
                    {
                        50, 0, 0, 0, 0, 5.510910450173815e-15, -89.99999761581421, 0, 0, 129.99999523162842,
                        7.960203902479246e-15, 0, 0, -0.0000095367431640625, 0, 1
                    }
                }
            },
            {.mesh = 3},
            {
                .matrix = {
                    {
                        39.99999761581421, 0, 0, 0, 0, 2.4492934523054316e-15, -39.99999761581421, 0, 0,
                        120.00000476837158, 7.347881086862668e-15, 0, 0, -0.0000095367431640625, 120.00000476837158, 1
                    }
                }
            },
            {.mesh = 4},
            {
                .matrix = {
                    {
                        50, 0, 0, 0, 0, 3.061616997868383e-15, -50, 0, 0, 129.99999523162842, 7.960203902479246e-15, 0,
                        0, -0.0000095367431640625, 120.00000476837158, 1
                    }
                }
            },
            {.mesh = 5},
            {
                .matrix = {
                    {
                        39.99999761581421, 0, 0, 0, 0, 2.4492934523054316e-15, -39.99999761581421, 0, 0,
                        120.00000476837158, 7.347881086862668e-15, 0, 0, -0.0000095367431640625, -120.00000476837158, 1
                    }
                }
            },
            {.mesh = 6},
            {
                .matrix = {
                    {
                        50, 0, 0, 0, 0, 3.061616997868383e-15, -50, 0, 0, 129.99999523162842, 7.960203902479246e-15, 0,
                        0, -0.0000095367431640625, -120.00000476837158, 1
                    }
                }
            },
            {.mesh = 7},
            {
                .matrix = {
                    {
                        39.99999761581421, 0, 0, 0, 0, 2.4492934523054316e-15, -39.99999761581421, 0, 0,
                        120.00000476837158, 7.347881086862668e-15, 0, 0, -240.00001907348633, -39.99999761581421, 1
                    }
                }
            },
            {.mesh = 8},
            {
                .matrix = {
                    {
                        50, 0, 0, 0, 0, 3.061616997868383e-15, -50, 0, 0, 129.99999523162842, 7.960203902479246e-15, 0,
                        0, -240.00001907348633, -39.99999761581421, 1
                    }
                }
            },
            {.mesh = 9},
            {
                .matrix = {
                    {
                        39.99999761581421, 0, 0, 0, 0, 2.4492934523054316e-15, -39.99999761581421, 0, 0,
                        120.00000476837158, 7.347881086862668e-15, 0, 0, -240.00001907348633, 39.99999761581421, 1
                    }
                }
            },
            {.mesh = 10},
            {
                .matrix = {
                    {
                        50, 0, 0, 0, 0, 3.061616997868383e-15, -50, 0, 0, 129.99999523162842, 7.960203902479246e-15, 0,
                        0, -240.00001907348633, 39.99999761581421, 1
                    }
                }
            },
            {.mesh = 11},
            {
                .children = {31, 40, 43}, .rotation = {{0.7071068286895752, 0, 0, 0.7071067094802856}},
                .translation = {{0, -1.0587911840678754e-22, 0.10000000149011612}}
            },
            {.children = {32, 34, 37}, .translation = {{0, 2.4000000953674316, 0}}},
            {.children = {33}, .translation = {{0, 2.4000000953674316, 0}}},
            {.translation = {{0, 0.8000001907348633, 0}}},
            {
                .children = {35},
                .rotation = {{-0.8112421631813049, 6.970288524144053e-8, -9.67075877156276e-8, 0.5847102999687195}},
                .scale = {{1, 0.9999999403953552, 0.9999999403953552}}, .translation = {{0, 2.4000000953674316, 0}}
            },
            {
                .children = {36},
                .rotation = {{-0.5847103595733643, 1.472805303279548e-14, -1.394058131154452e-7, 0.8112421035766602}},
                .translation = {{-3.552713678800501e-14, 1.2649112939834595, 2.384185791015625e-7}}
            },
            {
                .rotation = {{-6.617444900424222e-24, 2.6469779601696886e-23, -3.1554436208840472e-30, 1}},
                .translation = {{-5.293955920339377e-23, 2, 0}}
            },
            {
                .children = {38},
                .rotation = {{0.8112421631813049, 6.970284971430374e-8, 9.670753797763609e-8, 0.5847102999687195}},
                .scale = {{1, 0.9999999403953552, 0.9999999403953552}}, .translation = {{0, 2.4000000953674316, 0}}
            },
            {
                .children = {39},
                .rotation = {{0.5847103595733643, 4.93133840511438e-14, 1.3940578469373577e-7, 0.8112421035766602}},
                .translation = {{1.2079226507921703e-13, 1.2649112939834595, -2.384185791015625e-7}}
            },
            {
                .rotation = {{1.3234889800848443e-23, 5.293955920339377e-23, -4.733165431326071e-30, 1}},
                .translation = {{5.293955920339377e-23, 2, 0}}
            },
            {
                .children = {41}, .rotation = {{-0.7071068286895752, 0, 0, 0.7071067094802856}},
                .translation = {{0, 2.4000000953674316, 0}}
            },
            {
                .children = {42}, .rotation = {{-0.7071068286895752, 0, 0, 0.7071067094802856}},
                .translation = {{0, 0.4000000059604645, -2.220446049250313e-16}}
            },
            {
                .rotation = {{5.293955920339377e-23, 0, 0, 1}},
                .translation = {{0, 2.4000000953674316, -5.551115123125783e-17}}
            },
            {
                .children = {44},
                .rotation = {{0.7071068286895752, -3.090862676913275e-8, -3.090862321641907e-8, 0.7071067094802856}},
                .translation = {{0, 2.4000000953674316, 0}}
            },
            {
                .children = {45},
                .rotation = {{0.7071068286895752, 3.090862321641907e-8, -3.090862676913275e-8, 0.7071067094802856}},
                .translation = {{1.3234889800848443e-23, 0.4000000059604645, 0}}
            },
            {
                .rotation = {{0, 0, -3.76158192263132e-37, 1}},
                .translation = {{7.888609052210118e-31, 2.4000000953674316, 0}}
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

    CameraController c({}, 68);

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

        model.nodes[3].rotation = glm::rotate(glm::identity<glm::quat>(), -engine.frameInfo().time.count() * 2,
                                              glm::vec3{0, 1, 1});
        model.nodes[5].rotation = glm::rotate(glm::identity<glm::quat>(), engine.frameInfo().time.count() * 2,
                                              glm::vec3{0, 1, 1});

        for (const auto nodeIndex : model.scenes[model.scene].nodes)
            renderNode(model, nodeIndex, program, buffers);
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
