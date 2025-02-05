//
// Created by loumarti on 1/25/25.
//

#include "Animator.h"
#include "UserInterface.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Engine/Object.h"

UserInterface::UserInterface(Object& object, const Window& window) : EngineComponent(object),
                                                                     m_selected_animation(0),
                                                                     m_selected_golem_part(0),
                                                                     m_selected_golem_part_model_index(17)
{
    m_animator = &object.getComponent<Animator>()->get();

    m_animationsNames.reserve(m_animator->mesh().model().animations.size() + 1);
    m_animationsNames.push_back("-");
    for (const auto& animation : m_animator->mesh().model().animations)
        m_animationsNames.push_back(animation.name.c_str());

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


UserInterface::~UserInterface()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

auto UserInterface::newFrame() const -> void
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

auto UserInterface::setAnimationBlock() -> void
{
    ImGui::Text("Select animation");
    ImGui::Combo("#0", &m_selected_animation, m_animationsNames.data(), static_cast<int>(m_animationsNames.size()));
    ImGui::Dummy(ImVec2(s_text_offset, 0));
}

auto UserInterface::setGolemPartBlock() -> void
{
    const char* golem_parts[] = {
        "Head", "All",
        "Left arm", "Left arm lower", "Flower", "Left hand",
        "Right arm", "Right arm lower", "Right hand",
        "Left leg", "Left leg lower", "Left feet",
        "Right leg", "Right leg lower", "Right feet",
    };

    ImGui::Text("Select golem part");
    if (ImGui::Combo("#1", &m_selected_golem_part, golem_parts, IM_ARRAYSIZE(golem_parts)))
    {
        updatePartIndex();
        m_scale_x = 1.0f;
        m_scale_y = 1.0f;
        m_scale_z = 1.0f;
    }
    ImGui::Text("Customize scale");
    if (ImGui::InputFloat("x", &m_scale_x, 0.1f, 0.5f, "%.1f"))
    {
        if (m_scale_x < 0.0f)
            m_scale_x = 0.0f;
    }
    if (ImGui::InputFloat("y", &m_scale_y, 0.1f, 0.5f, "%.1f"))
    {
        if (m_scale_y < 0.0f)
            m_scale_y = 0.0f;
    }
    if (ImGui::InputFloat("z", &m_scale_z, 0.1f, 0.5f, "%.1f"))
    {
        if (m_scale_z < 0.0f)
            m_scale_z = 0.0f;
    }
}

auto UserInterface::setCustomGolemPartBlock() -> void
{
    ImGui::Text("Custom node selection");
    ImGui::Text("Try a node index");
    if (ImGui::InputInt("#2", &m_custom_golem_part_model_index))
    {
        if (m_custom_golem_part_model_index < 0)
            m_custom_golem_part_model_index = 0;
        if (m_custom_golem_part_model_index > s_golem_node_max_index)
            m_custom_golem_part_model_index = s_golem_node_max_index;
        m_custom_scale_x = 1.0f;
        m_custom_scale_y = 1.0f;
        m_custom_scale_z = 1.0f;
    }
    ImGui::Text("Customize scale");
    if (ImGui::InputFloat("x'", &m_custom_scale_x, 0.1f, 0.5f, "%.1f"))
    {
        if (m_custom_scale_x < 0.0f)
            m_custom_scale_x = 0.0f;
    }
    if (ImGui::InputFloat("y'", &m_custom_scale_y, 0.1f, 0.5f, "%.1f"))
    {
        if (m_custom_scale_y < 0.0f)
            m_custom_scale_y = 0.0f;
    }
    if (ImGui::InputFloat("z'", &m_custom_scale_z, 0.1f, 0.5f, "%.1f"))
    {
        if (m_custom_scale_z < 0.0f)
            m_custom_scale_z = 0.0f;
    }
}

auto UserInterface::setDisplayModeBlock() -> void
{
    // if (ImGui::Checkbox("Display", &m_fill_mode))
    //     m_fill_mode ? Engine::useFillDisplayMode() : Engine::useLineDisplayMode();
    ImGui::SameLine();
    ImGui::Text("%s", m_fill_mode ? "fill mode" : "line mode");
}

auto UserInterface::sectionSeparator() const -> void
{
    ImGui::Dummy(ImVec2(0, s_section_padding));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, s_section_padding));
}

auto UserInterface::updatePartIndex() -> void
{
    /*
    const char* golem_parts[] = {
        "Head", "All",
           17     1
        "Left arm", "Left arm lower", "Flower", "Left hand",
            75              90            101        95
        "Right arm", "Right arm lower", "Right hand",
            28              48               63
        "Left leg", "Left leg lower", "Left feet",
            107             116            121
        "Right leg", "Right leg lower", "Right feet",
            128            135              138
    };
    */
    switch (m_selected_golem_part)
    {
    case 0:
        m_selected_golem_part_model_index = 17;
        break;
    case 1:
        m_selected_golem_part_model_index = 1;
        break;
    case 2:
        m_selected_golem_part_model_index = 75;
        break;
    case 3:
        m_selected_golem_part_model_index = 90;
        break;
    case 4:
        m_selected_golem_part_model_index = 101;
        break;
    case 5:
        m_selected_golem_part_model_index = 95;
        break;
    case 6:
        m_selected_golem_part_model_index = 28;
        break;
    case 7:
        m_selected_golem_part_model_index = 48;
        break;
    case 8:
        m_selected_golem_part_model_index = 63;
        break;
    case 9:
        m_selected_golem_part_model_index = 107;
        break;
    case 10:
        m_selected_golem_part_model_index = 116;
        break;
    case 11:
        m_selected_golem_part_model_index = 121;
        break;
    case 12:
        m_selected_golem_part_model_index = 128;
        break;
    case 13:
        m_selected_golem_part_model_index = 135;
        break;
    case 14:
        m_selected_golem_part_model_index = 138;
        break;
    default:
        m_selected_golem_part_model_index = 17;
        break;
    }
}

auto UserInterface::onUpdate(Engine& engine) -> void
{
    newFrame();
    constexpr auto windowSize = ImVec2(s_frame_width, s_frame_height);
    constexpr auto windowPos = ImVec2(s_frame_x, s_frame_y);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);

    ImGui::Begin("GolemGL");

    setAnimationBlock();
    sectionSeparator();
    setGolemPartBlock();
    sectionSeparator();
    setCustomGolemPartBlock();
    sectionSeparator();
    setDisplayModeBlock();

    // interface index for "none" is 0, and -1 for animator
    if ((m_selected_animation - 1) != m_animator->currentAnimationIndex())
        m_animator->setAnimation(m_selected_animation - 1);

    ImGui::End();
}

auto UserInterface::onPostRender(Engine& engine) -> void
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

auto UserInterface::get_golem_animation_name(int index) -> std::string
{
    switch (index)
    {
    case 0:
        return "run 1";
    case 1:
        return "death 2";
    case 2:
        return "run 2";
    case 3:
        return "sit";
    case 4:
        return "walk 2";
    case 5:
        return "stopping";
    case 6:
        return "death 1";
    case 7:
        return "idle 1";
    case 8:
        return "idle flower";
    case 9:
        return "flower";
    case 10:
        return "stand up";
    default:
        return "unknown animation";
    }
}
