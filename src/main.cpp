#include <fstream>
#include <iostream>

#include "GLFW/glfw3.h"

#include "HumanGLConfig.h"
#include "Engine/Engine.h"
#include "Window/Window.h"
#include "WindowContext.h"
#include "Components/UserInterface.h"
#include "Components/CameraController.h"
#include "Components/MeshRenderer.h"
#include "Models/Frog.microgltf.h"
#include "Models/Golem.microgltf.h"
#include "Models/Village.microgltf.h"

GLuint whiteTexture = 0;

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
    auto shader = *engine.makeShaderVariants("default", RESOURCE_PATH"shaders/default.vert",
                                             RESOURCE_PATH"shaders/default.frag");
    shader.get().enableVariant(ShaderHasNone);

    auto frogMicrogltfLoaded = frogMicrogltf; // TODO TMP
    for (auto& buffer : frogMicrogltfLoaded.buffers)
    {
        if (!buffer.data.empty())
            continue;

        buffer.data = readFileToVector(RESOURCE_PATH"models/frog_jumping/" + buffer.uri, buffer.byteLength);
    }

    auto golemMicrogltfLoaded = golemMicrogltf; // TODO TMP
    for (auto& buffer : golemMicrogltfLoaded.buffers)
    {
        if (!buffer.data.empty())
            continue;

        buffer.data = readFileToVector(RESOURCE_PATH"models/iron_golem/" + buffer.uri, buffer.byteLength);
    }

    auto villageMicrogltfLoaded = villageMicrogltf; // TODO TMP
    for (auto& buffer : villageMicrogltfLoaded.buffers)
    {
        if (!buffer.data.empty())
            continue;

        buffer.data = readFileToVector(RESOURCE_PATH"models/minecraft_village/" + buffer.uri, buffer.byteLength);
    }

    // TODO don't ignore expected
    auto frogMesh = *engine.loadModel("frog_jumping", frogMicrogltfLoaded);
    auto golemMesh = *engine.loadModel("iron_golem", golemMicrogltfLoaded);
    auto villageMesh = *engine.loadModel("minecraft_village", villageMicrogltfLoaded);

    std::ignore = golemMesh.get().prepareShaderPrograms(shader.get());
    std::ignore = villageMesh.get().prepareShaderPrograms(shader.get());

    {
        // Frog 1
        auto& object = engine.instantiate();
        object.transform().translation.x = -2.5;
        object.transform().translation.z = 1.5;
        auto& animator = object.addComponent<Animator>(frogMesh);
        auto& meshRenderer = object.addComponent<MeshRenderer>(frogMesh, shader);
        meshRenderer.setAnimator(animator);
        animator.setAnimation(0);
    }

    {
        // Frog 2
        auto& object = engine.instantiate();
        object.transform().translation.x = 1.5;
        object.transform().translation.z = 8;
        auto& animator = object.addComponent<Animator>(frogMesh);
        auto& meshRenderer = object.addComponent<MeshRenderer>(frogMesh, shader);
        meshRenderer.setAnimator(animator);
        animator.setAnimation(0);
    }

    {
        // Frog 3
        auto& object = engine.instantiate();
        object.transform().translation.x = 0;
        object.transform().translation.y = 1;
        object.transform().translation.z = -5;
        auto& animator = object.addComponent<Animator>(frogMesh);
        auto& meshRenderer = object.addComponent<MeshRenderer>(frogMesh, shader);
        meshRenderer.setAnimator(animator);
        animator.setAnimation(0);
    }

    {
        // Golem
        auto& object = engine.instantiate();
        auto& animator = object.addComponent<Animator>(golemMesh);
        auto& meshRenderer = object.addComponent<MeshRenderer>(golemMesh, shader);
        meshRenderer.setAnimator(animator);
        animator.setAnimation(7);
        auto windowData = ImguiWindowData(8, 156, 230, 280);
        object.addComponent<UserInterface>(engine.getWindow(), "Golem", windowData);
    // golemObject.addComponent<GolemUserInterface>(engine.getWindow());
    }

    {
        // Village
        auto& object = engine.instantiate();
        object.addComponent<MeshRenderer>(villageMesh, shader);
        object.addComponent<UserInterface>(engine.getWindow(), "Village");
        object.transform().translation = ft_glm::vec3(-4.2, 8.11, -4);
        object.transform().scale = ft_glm::vec3(1.5f);
    }

    {
        // Camera
        auto& object = engine.instantiate();
        auto& camera = object.addComponent<Camera>(WIDTH, HEIGHT, 60);
        object.addComponent<CameraController>(ft_glm::vec3{0, 1.4, 0}, 5);
        engine.setCamera(camera);
    }

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
