//
// Created by Simon Cros on 1/13/25.
//

#include "Camera.h"
#include "Engine.h"
#include "OpenGL/Debug.h"

auto Engine::Create(Window&& window) -> Engine
{
    return Engine(std::move(window));
}

Engine::Engine(Window&& window) noexcept :
    m_window(std::move(window))
{
    m_window.setAsCurrentContext();
    const int version = gladLoadGL(glfwGetProcAddress);
    std::cout << "OpenGL " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;

    const bool hasDebugOutput = GLAD_GL_KHR_debug || GLAD_GL_ARB_debug_output;

    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    glEnable(GL_DEPTH_TEST);
    if (hasDebugOutput && (flags & GL_CONTEXT_FLAG_DEBUG_BIT))
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }

    // TODO TMP only one global VAO for testing
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

auto Engine::run() -> void
{
    assert(m_camera != nullptr && "Camera is null");

    if (m_doubleSided)
        glDisable(GL_CULL_FACE);
    else
        glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, m_polygonMode);

    glClearColor(0.4705882353f, 0.6549019608f, 1.0f, 1.0f);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    m_start = ClockType::now();

    auto previousTime = m_start;
    while (m_window.update())
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto pvMat = m_camera->projectionMatrix() * m_camera->computeViewMatrix();
        for (auto& [id, shader] : m_shaders)
        {
            for (auto& [flags, variant] : shader->programs)
            {
                useProgram(*variant.get());
                variant.get()->setMat4("u_projectionView", pvMat);
            }
        }

        for (const auto& object : m_objects)
            object->update(*this);

        m_window.swapBuffers();

        const auto newTime = ClockType::now();
        ++m_currentFrameInfo.frameCount;
        m_currentFrameInfo.time = newTime - m_start;
        m_currentFrameInfo.deltaTime = newTime - previousTime;
        previousTime = newTime;
    }
}

auto Engine::makeShaderVariants(const std::string_view& id, const std::string& vertPath,
                                const std::string& fragPath) -> Expected<ShaderProgramVariantsRef, std::string>
{
    auto e_shaderVariants = ShaderProgram::Create(vertPath, fragPath);
    if (!e_shaderVariants)
        return Unexpected(std::move(e_shaderVariants).error());
    
    // C++ 26 will avoid new key allocation if key already exist (remove explicit std::string constructor call).
    // In this function, unnecessary string allocation is not really a problem since we should not try to add two shaders with the same id
    auto [it, inserted] = m_shaders.try_emplace(std::string(id),
                                               std::make_unique<ShaderProgram>(*std::move(e_shaderVariants)));

    if (!inserted)
        return Unexpected("A shader with the same id already exist");
    return *it->second;
}

auto Engine::loadModel(const std::string_view& id, const microgltf::Model& gltfModel) -> Expected<ModelRef, std::string>
{
    auto model = Mesh::Create(std::string(id), gltfModel, *m_shaders["default"]);

    // C++ 26 will avoid new key allocation if key already exist (remove explicit std::string constructor call).
    // In this function, unnecessary string allocation is not really a problem since we should not try to add two shaders with the same id
    auto [it, inserted] = m_models.try_emplace(std::string(id), std::make_unique<Mesh>(std::move(model)));

    if (!inserted)
        return Unexpected("A model with the same id already exist");
    return *it->second;
}

auto Engine::instantiate() -> Object&
{
    return **m_objects.emplace(std::make_unique<Object>()).first;
}
