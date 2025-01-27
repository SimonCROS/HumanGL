#include <iostream>

#include "glad/gl.h"
#include "GLFW/glfw3.h"

#include "HumanGLConfig.h"
#include "Engine/Engine.h"
#include "Scripts/UserInterface.h"
#include "Window/Window.h"
#include "WindowContext.h"
#include "Engine/Animation.h"
#include "glm/gtx/string_cast.hpp"
#include "MicroGLTF/Struct.h"
#include "OpenGL/ShaderProgramVariants.h"
#include "Scripts/CameraController.h"
#include "Golem.microgltf.h"

GLuint whiteTexture = 0;

#include <fstream>

auto readFileToVector(const std::string& filename, const std::streamsize fileSize) -> std::vector<uint8_t>
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

auto createWhiteTexture() -> GLuint
{
    constexpr int data[4] = {~0, ~0, ~0, ~0};
    GLuint whiteTextureId;
    glGenTextures(1, &whiteTextureId);
    glBindTexture(GL_TEXTURE_2D, whiteTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    return whiteTextureId;
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

    auto engine = Engine::Create(*std::move(e_window));
    engine.getWindow().setKeyCallback([](const Window& window, const int key, const int action, int mode) -> void
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            window.setShouldClose();
    });

    microgltf::Model golem = golemMicrogltf;
    for (auto& buffer : golem.buffers)
    {
        if (!buffer.data.empty())
            continue;
        buffer.data = readFileToVector(RESOURCE_PATH"models/iron_golem/" + buffer.uri, buffer.byteLength);
    }

    engine.create<UserInterface>(engine.getWindow());
    engine.create<CameraController>(glm::vec3{0, 3.5, 0}, 50);

    // engine.makeShader(RESOURCE_PATH "shaders/default.vert", RESOURCE_PATH "shaders/default.frag");
    // engine.loadModel(golem);

    engine.run();

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
