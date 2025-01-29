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

    // TODO don't ignore expected
    auto mesh = *engine.loadModel("golem", golemMicrogltf);

    const auto golemObject = engine.instantiate();
    golemObject.get().addComponent<MeshRenderer>(mesh, shader);
    // golemObject.get().addComponent<UserInterface>();

    // engine.mainCamera().addComponent<CameraController>(glm::vec3{0, 3.5, 0}, 50);

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
