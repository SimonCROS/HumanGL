#include <fstream>
#include <iostream>

#include "GLFW/glfw3.h"

#include "HumanGLConfig.h"
#include "Engine/Engine.h"
#include "Window/Window.h"
#include "WindowContext.h"
#include "Components/UserInterface.h"
#include "Components/CameraController.h"
#include "Components/ImguiSingleton.h"
#include "Components/MeshRenderer.h"
#include "InterfaceBlocks/CameraTargetInterfaceBlock.h"
#include "InterfaceBlocks/DisplayInterfaceBlock.h"
#include "InterfaceBlocks/GolemInterfaceBlock.h"
#include "Models/Frog.microgltf.h"
#include "Models/Golem.microgltf.h"
#include "Models/Village.microgltf.h"

GLuint whiteTexture = 0;

auto readFileToVector(const std::string& filename, const std::streamsize fileSize) -> std::vector<uint8_t>
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Failed to open file " + filename);
    }

    std::vector<uint8_t> buffer(fileSize);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);

    if (!file)
    {
        throw std::runtime_error("Failed to read file " + filename);
    }

    return buffer;
}

auto fillUpBuffer(microgltf::Model &model, std::string const &fileRelativePath) -> Expected<void, std::string>
{
    for (auto& [data, byteLength, uri] : model.buffers)
    {
        if (!data.empty())
            continue;
        try
        {
            data = readFileToVector(RESOURCE_PATH + fileRelativePath + uri, byteLength);
        } catch (const std::exception& e) {
            return Unexpected(e.what());
        }
    }
    return {};
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
    auto e_frogBuffer = fillUpBuffer(frogMicrogltfLoaded, "models/frog_jumping/");
    if (!e_frogBuffer)
        return Unexpected(std::move(e_frogBuffer).error());

    auto golemMicrogltfLoaded = golemMicrogltf; // TODO TMP
    auto e_golemBuffer = fillUpBuffer(golemMicrogltfLoaded, "models/iron_golem/");
    if (!e_golemBuffer)
        return Unexpected(std::move(e_golemBuffer).error());

    auto villageMicrogltfLoaded = villageMicrogltf; // TODO TMP
    auto e_villageBuffer = fillUpBuffer(villageMicrogltfLoaded, "models/minecraft_village/");
    if (!e_villageBuffer)
        return Unexpected(std::move(e_villageBuffer).error());

    // TODO don't ignore expected
    auto frogMesh = *engine.loadModel("frog_jumping", frogMicrogltfLoaded);
    auto golemMesh = *engine.loadModel("iron_golem", golemMicrogltfLoaded);
    auto villageMesh = *engine.loadModel("minecraft_village", villageMicrogltfLoaded);

    std::ignore = golemMesh.get().prepareShaderPrograms(shader.get());
    std::ignore = villageMesh.get().prepareShaderPrograms(shader.get());

    {
        // Imgui object
        auto& object = engine.instantiate();
        object.addComponent<ImguiSingleton>(engine.getWindow());
    }

    CameraController* cameraController;
    {
        // Camera
        auto& object = engine.instantiate();
        const auto& camera = object.addComponent<Camera>(WIDTH, HEIGHT, 60);
        cameraController = &object.addComponent<CameraController>(ft_glm::vec3{0, 1.4, 0}, 5);
        engine.setCamera(camera);
    }

    {
        // Frog 1
        auto& object = engine.instantiate();
        object.transform().translation.x = -2.5;
        object.transform().translation.z = 1.5;
        auto& animator = object.addComponent<Animator>(frogMesh);
        auto& meshRenderer = object.addComponent<MeshRenderer>(frogMesh, shader);
        meshRenderer.setAnimator(animator);
        animator.setAnimation(0);
        constexpr auto windowData = ImguiWindowData{
            .s_frame_x = WIDTH - 8 - 230, .s_frame_y = 8, .s_frame_width = 230, .s_frame_height = 125
        };
        auto& interface = object.addComponent<UserInterface>("Frog 1", windowData);
        interface.addBlock<CameraTargetInterfaceBlock>(1, *cameraController, 1);
        interface.addBlock<DisplayInterfaceBlock>(10);
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
        constexpr auto windowData = ImguiWindowData{
            .s_frame_x = WIDTH - 8 - 230, .s_frame_y = 8 + 125 + 8, .s_frame_width = 230, .s_frame_height = 125
        };
        auto& interface = object.addComponent<UserInterface>("Frog 2", windowData);
        interface.addBlock<CameraTargetInterfaceBlock>(1, *cameraController, 1);
        interface.addBlock<DisplayInterfaceBlock>(10);
    }

    {
        // Frog 3
        auto& object = engine.instantiate();
        object.transform().translation.x = 0;
        object.transform().translation.y = 0.74;
        object.transform().translation.z = -5.5;
        auto& animator = object.addComponent<Animator>(frogMesh);
        auto& meshRenderer = object.addComponent<MeshRenderer>(frogMesh, shader);
        meshRenderer.setAnimator(animator);
        animator.setAnimation(0);
        constexpr auto windowData = ImguiWindowData{
            .s_frame_x = WIDTH - 8 - 230, .s_frame_y = 8 + 125 + 8 + 125 + 8, .s_frame_width = 230,
            .s_frame_height = 125
        };
        auto& interface = object.addComponent<UserInterface>("Frog 3", windowData);
        interface.addBlock<CameraTargetInterfaceBlock>(1, *cameraController, 1);
        interface.addBlock<DisplayInterfaceBlock>(10);
    }

    {
        // Golem
        auto& object = engine.instantiate();
        auto& animator = object.addComponent<Animator>(golemMesh);
        auto& meshRenderer = object.addComponent<MeshRenderer>(golemMesh, shader);
        meshRenderer.setAnimator(animator);
        animator.setAnimation(7);
        constexpr auto windowData = ImguiWindowData{
            .s_frame_x = 8, .s_frame_y = 8 + 125 + 8, .s_frame_width = 230,
            .s_frame_height = 400
        };
        auto& interface = object.addComponent<UserInterface>("Golem", windowData);
        interface.addBlock<CameraTargetInterfaceBlock>(1, *cameraController, 5);
        interface.addBlock<DisplayInterfaceBlock>(10);
        interface.addBlock<GolemInterfaceBlock>(100);
    }

    {
        // Village
        auto& object = engine.instantiate();
        object.addComponent<MeshRenderer>(villageMesh, shader);
        object.transform().translation = ft_glm::vec3(-4.2, 8.11, -4);
        object.transform().scale = ft_glm::vec3(1.5f);
        constexpr auto windowData = ImguiWindowData{
            .s_frame_x = 8, .s_frame_y = 8, .s_frame_width = 230,
            .s_frame_height = 125
        };
        auto& interface = object.addComponent<UserInterface>("Village", windowData);
        interface.addBlock<CameraTargetInterfaceBlock>(1, *cameraController, 20);
        interface.addBlock<DisplayInterfaceBlock>(10);
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
