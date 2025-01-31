#include <iostream>

#include "GLFW/glfw3.h"

#include "HumanGLConfig.h"
#include "Engine/Engine.h"
#include "Components/UserInterface.h"
#include "Window/Window.h"
#include "WindowContext.h"
#include "glm/gtx/string_cast.hpp"
#include "Components/CameraController.h"
#include "Golem.microgltf.h"

#include <fstream>

#include "Components/MeshRenderer.h"

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

    // TODO don't ignore expected
    auto shader = *engine.makeShaderVariants("default", RESOURCE_PATH"shaders/default.vert", RESOURCE_PATH"shaders/default.frag");
    shader.get().enableVariant(ShaderHasNone);

    auto golemMicrogltfLoaded = golemMicrogltf; // TODO TMP
    for (auto& buffer : golemMicrogltfLoaded.buffers)
    {
        if (!buffer.data.empty())
            continue;

        buffer.data = readFileToVector(RESOURCE_PATH"models/iron_golem/" + buffer.uri, buffer.byteLength);
    }

    // TODO don't ignore expected
    auto mesh = *engine.loadModel("golem", golemMicrogltfLoaded);

    const auto golemObject = engine.instantiate();
    golemObject.get().addComponent<MeshRenderer>(mesh, shader);
    golemObject.get().addComponent<UserInterface>(engine.getWindow());

    const auto cameraHolder = engine.instantiate();
    const auto camera = cameraHolder.get().addComponent<Camera>(WIDTH, HEIGHT, 60);
    cameraHolder.get().addComponent<CameraController>(glm::vec3{0, 3.5, 0}, 50);
    engine.setCamera(camera);

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
