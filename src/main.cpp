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
#include "OpenGL/IndicesBuffer.h"
#include "OpenGL/Shader.h"
#include "OpenGL/ShaderProgramVariants.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "Scripts/CameraController.h"

constexpr GLuint indices[] = {3, 2, 6, 7, 4, 2, 0, 3, 1, 6, 5, 4, 1, 0};
constexpr GLfloat vertex[] = {
    -1, -1, 1, //  0 : Front-bottom-left
    1, -1, 1, //   1 : Front-bottom-right
    1, 1, 1, //    2 : Front-top-right
    -1, 1, 1, //   3 : Front-top-left
    -1, -1, -1, // 4 : Back-bottom-left
    1, -1, -1, //  5 : Back-bottom-right
    1, 1, -1, //   6 : Back-top-right
    -1, 1, -1, //  7 : Back-top-left
};
constexpr size_t indicesViewSize = sizeof(indices);
constexpr size_t vertexViewSize = sizeof(vertex);
constexpr size_t indicesCount = indicesViewSize / sizeof(*indices);
constexpr size_t vertexCount = vertexViewSize / sizeof(*vertex) / 3;
constexpr size_t bufferSize = indicesViewSize + vertexViewSize;

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

    microgltf::Model models[] = {
        {
            .scene = 1,
            .scenes = {
                {
                    .nodes = {0},
                }
            },
            .nodes = {
                {
                    .children = {1, 2, 3, 4, 5, 6},
                    .name = "",
                },
                {
                    .mesh = 0,
                    .scale = {1, 2, 0.5},
                    .name = "body",
                },
                {
                    .mesh = 0,
                    .translation = {0, 2, 0},
                    .name = "head",
                },
                {
                    .mesh = 0,
                    .scale = {0.4, 2, 0.5},
                    .translation = {-1, -1, 0},
                    .name = "arm_left",
                },
                {
                    .mesh = 0,
                    .scale = {0.4, 2, 0.5},
                    .translation = {1, -1, 0},
                    .name = "arm_right",
                },
                {
                    .mesh = 0,
                    .scale = {0.4, 2, 0.5},
                    .translation = {-0.4, -2, 0},
                    .name = "leg_left",
                },
                {
                    .mesh = 0,
                    .scale = {0.4, 2, 0.5},
                    .translation = {0.4, -2, 0},
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
                    .componentType = GL_FLAT,
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
                    .target = GL_ELEMENT_ARRAY_BUFFER,
                },
            },
            .buffers = {
                {
                    .content = std::move(cubeBufferData),
                },
            },
        },
    };

    ShaderProgramVariants programVariants(RESOURCE_PATH "shaders/default.vert", RESOURCE_PATH "shaders/default.frag");
    programVariants.enableVariant(ShaderHasNone);
    ShaderProgram program = programVariants.getProgram(ShaderHasNone); // ONLY USE ONE SHADER

    auto vertexArray = VertexArray();
    vertexArray.bind();

    CameraController c({}, 15);

    glClearColor(0.7f, 0.9f, 0.1f, 1.0f);
    engine.run([&](Engine& engine)
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // Will be automatized
        c.update(engine.getWindow().getCurrentControls(), camera, engine.getFrameInfo().deltaTime.count());

        const auto pvMat = camera.projectionMatrix() * camera.computeViewMatrix();
        program.use();
        program.setMat4("projectionView", pvMat);
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
