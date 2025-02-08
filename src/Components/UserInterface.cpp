//
// Created by loumarti on 1/25/25.
//

#include "Animator.h"
#include "MeshRenderer.h"
#include "UserInterface.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Engine/Object.h"

std::size_t UserInterface::s_nextID = 0;

UserInterface::UserInterface(Object& object, const Window& window, const std::string_view name, const ImguiWindowData &windowData)
: EngineComponent(object), m_id(generateUniqueID()), m_windowData(windowData)
{
    m_meshRenderer = &object.getComponent<MeshRenderer>()->get();
    m_name = name;

    if (s_instanceCount == 0)
    {
        initializeImGui(window);
        s_instanceCount++;
    }
}

UserInterface::~UserInterface()
{
    if (s_instanceCount <= 0)
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
    s_instanceCount--;
}

auto UserInterface::newFrame() const -> void
{
    s_renderCalled = false;
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

auto UserInterface::setDisplayModeBlock(Engine &engine) -> void
{
    constexpr const char* displayModes[] = {
        "Fill", "Line", "Point",
    };
    constexpr int displayModeToPolygonMode[] = {
        GL_FILL, GL_LINE, GL_POINT,
    };

    ImGui::Text("Select display mode");
    if (ImGui::Combo(makeLabel("#0").c_str(), &m_selectedDisplayMode, displayModes, IM_ARRAYSIZE(displayModes)))
        m_meshRenderer->setPolygoneMode(engine, displayModeToPolygonMode[m_selectedDisplayMode]);
        // engine.setPolygoneMode(displayModeToPolygonMode[m_selectedDisplayMode]);
}

auto UserInterface::makeLabel(std::string_view const prefix) const -> std::string
{
    return std::string(prefix) + "_" + std::to_string(m_id);
}

auto UserInterface::setDisplayBlock() -> void
{
    if (ImGui::Checkbox("Display", &m_display))
        m_meshRenderer->setDisplay(m_display);
}

auto UserInterface::addSectionSeparator() const -> void
{
    ImGui::Dummy(ImVec2(0, s_section_padding));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, s_section_padding));
}

auto UserInterface::initializeImGui(const Window& window) -> void
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = "lib/imgui/cache/imgui.ini";
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.getGLFWHandle(), true);
    ImGui_ImplOpenGL3_Init();
}

auto UserInterface::onUpdate(Engine& engine) -> void
{
    if (!s_newFrameCalled)
    {
        newFrame();
        s_newFrameCalled = true;
    }

    s_renderCalled = false;
    auto windowSize = ImVec2(static_cast<float>(m_windowData.s_frame_width), static_cast<float>(m_windowData.s_frame_height));
    auto windowPos = ImVec2(static_cast<float>(m_windowData.s_frame_x), static_cast<float>(m_windowData.s_frame_y));
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);

    ImGui::Begin(m_name.c_str());

    // setAnimationBlock();
    // addSectionSeparator();
    // setGolemPartBlock();
    // addSectionSeparator();
    setDisplayModeBlock(engine);
    addSectionSeparator();
    setDisplayBlock();

    ImGui::End();
}

auto UserInterface::onPostRender(Engine& engine) -> void
{
    if (!s_renderCalled)
    {
        s_renderCalled = true;
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        s_newFrameCalled = false;
    }
}
