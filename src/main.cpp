#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "HumanGLConfig.h"
#include "Camera.h"
#include "Engine.h"
#include "Window.h"
#include "WindowContext.h"
#include "Engine/Cube.h"
#include "MicroGLTF/Model.h"
#include "OpenGL/Shader.h"
#include "OpenGL/ShaderProgramVariants.h"
#include "OpenGL/VertexArray.h"
#include "Scripts/CameraController.h"

constexpr GLuint indices[] = {3, 2, 6, 7, 4, 2, 0, 3, 1, 6, 5, 4, 1, 0};
constexpr GLfloat vertex[] = {
    -0.5, -0.5, 0.5, //  0 : Front-bottom-left
    0.5, -0.5, 0.5, //   1 : Front-bottom-right
    -0.5, 0.5, 0.5, //   2 : Front-top-left
    0.5, 0.5, 0.5, //    3 : Front-top-right
    -0.5, -0.5, -0.5, // 4 : Back-bottom-left
    0.5, -0.5, -0.5, //  5 : Back-bottom-right
    0.5, 0.5, -0.5, //   6 : Back-top-right
    -0.5, 0.5, -0.5, //  7 : Back-top-left
};
constexpr size_t indicesViewSize = sizeof(indices);
constexpr size_t vertexViewSize = sizeof(vertex);
constexpr size_t indicesCount = indicesViewSize / sizeof(*indices);
constexpr size_t vertexCount = vertexViewSize / sizeof(*vertex) / 3;
constexpr size_t bufferSize = indicesViewSize + vertexViewSize;

constexpr void* bufferOffset(const size_t offset)
{
    return reinterpret_cast<void*>(offset);
}

auto renderMesh(const microgltf::Model& model, const int meshIndex, ShaderProgram& program, const std::unordered_map<size_t, GLuint>& buffers, glm::mat4 transform) -> void
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
                const std::unordered_map<size_t, GLuint>& buffers, glm::mat4 transform = glm::identity<glm::mat4>()) -> void
{
    const microgltf::Node& node = model.nodes[nodeIndex];

    if (node.translation.has_value())
        transform = glm::translate(transform, *node.translation);

    if (node.rotation.has_value())
        transform *= glm::mat4_cast(*node.rotation);

    if (node.scale.has_value())
        transform = glm::scale(transform, *node.scale);

    if (node.mesh > -1)
        renderMesh(model, node.mesh, program, buffers, transform);
    for (const auto childIndex : node.children)
        renderNode(model, childIndex, program, buffers, transform);
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
                    glBufferData(bufferView.target, bufferView.byteLength, &buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

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

    std::vector<GLubyte> cubeBufferData;
    cubeBufferData.reserve(bufferSize);
    cubeBufferData.insert(cubeBufferData.end(), reinterpret_cast<const GLubyte*>(indices),
                          reinterpret_cast<const GLubyte*>(indices) + indicesViewSize);
    cubeBufferData.insert(cubeBufferData.end(), reinterpret_cast<const GLubyte*>(vertex),
                          reinterpret_cast<const GLubyte*>(vertex) + vertexViewSize);

    microgltf::Model model = {
        .scene = 0,
        .scenes = {
            {
                .nodes = {0},
            }
        },
        .nodes = {
            {
                .children = {1, 2, 3, 5, 7, 9},
                .name = "",
            },
            {
                .mesh = 0,
                .scale = {{1, 1.5, 0.5}},
                .translation = {{0, 0.75, 0}},
                .name = "body",
            },
            {
                .mesh = 0,
                .scale = {{1, 1, 1}},
                .translation = {{0, 2.25, 0}},
                .name = "head",
            },
            {
                .children = {4},
                .translation = {{-0.75, 1.5, 0}},
                .name = "arm_left_pivot",
            },
            {
                .mesh = 0,
                .scale = {{0.5, 1.5, 0.5}},
                .translation = {{-0.25, -0.75, 0}},
                .name = "arm_left",
            },
            {
                .children = {6},
                .translation = {{0.75, 1.5, 0}},
                .name = "arm_right_pivot",
            },
            {
                .mesh = 0,
                .scale = {{0.5, 1.5, 0.5}},
                .translation = {{0.25, -0.75, 0}},
                .name = "arm_right",
            },
            {
                .children = {8},
                .translation = {{-0.25, -0.75, 0}},
                .name = "leg_left_pivot",
            },
            {
                .mesh = 0,
                .scale = {{0.5, 1.5, 0.5}},
                .translation = {{-0.25, -0.75, 0}},
                .name = "leg_left",
            },
            {
                .children = {10},
                .translation = {{0.25, -0.75, 0}},
                .name = "leg_right_pivot",
            },
            {
                .mesh = 0,
                .scale = {{0.5, 1.5, 0.5}},
                .translation = {{0.25, -0.75, 0}},
                .name = "leg_right",
            },
        },
        .meshes = {
            {
                .primitives = {
                    {
                        .indices = 0,
                        .attributes = {
                            {"POSITION", 1},
                        },
                        .mode = GL_TRIANGLE_STRIP,
                    },
                },
            },
        },
        .accessors = {
            {
                .bufferView = 0,
                .byteOffset = 0,
                .type = microgltf::Scalar,
                .componentType = GL_UNSIGNED_INT,
                .count = indicesCount,
            },
            {
                .bufferView = 1,
                .byteOffset = 0,
                .type = microgltf::Vec3,
                .componentType = GL_FLOAT,
                .count = vertexCount,
            },
        },
        .bufferViews = {
            {
                .buffer = 0,
                .byteOffset = 0,
                .byteLength = indicesViewSize,
                .byteStride = 0,
                .target = GL_ELEMENT_ARRAY_BUFFER,
            },
            {
                .buffer = 0,
                .byteOffset = indicesViewSize,
                .byteLength = vertexViewSize,
                .byteStride = sizeof(*vertex) * 3,
                .target = GL_ARRAY_BUFFER,
            },
        },
        .buffers = {
            {
                .data = std::move(cubeBufferData),
            },
        },
    };

    ShaderProgramVariants programVariants(RESOURCE_PATH "shaders/default.vert", RESOURCE_PATH "shaders/default.frag");
    programVariants.enableVariant(ShaderHasNone);
    ShaderProgram program = programVariants.getProgram(ShaderHasNone); // ONLY USE ONE SHADER

    auto vertexArray = VertexArray();

    std::unordered_map<size_t, GLuint> buffers;
    prepare(model, vertexArray, buffers);

    CameraController c({}, 8);

    vertexArray.bind();
    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    engine.run([&](Engine& engine)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Will be automatized
        c.update(engine.getWindow().getCurrentControls(), camera, engine.frameInfo().deltaTime.count());

        const auto pvMat = camera.projectionMatrix() * camera.computeViewMatrix();
        program.use();
        program.setMat4("projectionView", pvMat);

        model.nodes[3].rotation = glm::rotate(glm::identity<glm::quat>(), -engine.frameInfo().time.count() * 2, glm::vec3{0,1,1});
        model.nodes[5].rotation = glm::rotate(glm::identity<glm::quat>(), engine.frameInfo().time.count() * 2, glm::vec3{0,1,1});

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
