//
// Created by loumarti on 1/25/25.
//

#include "UserInterface.h"

UserInterface::UserInterface(GLFWwindow* window) : m_selected_animation(0), m_selected_golem_part(0)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = "lib/imgui/cache/imgui.ini";
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}


UserInterface::~UserInterface()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

auto UserInterface::newFrame() -> void
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

auto UserInterface::setAnimationBlock() -> void
{
    ImGui::Text("Select animation");
    if (ImGui::InputInt("#0", &m_selected_animation, 1, 5)) {
        if (m_selected_animation < 0)
            m_selected_animation = 0;
        if (m_selected_animation > s_animation_max_index)
            m_selected_animation = s_animation_max_index;
    }
    ImGui::Dummy(ImVec2(s_text_offset, 0));
    ImGui::SameLine();
    ImGui::Text("%s", get_golem_animation_name(m_selected_animation).c_str());
}

auto UserInterface::setGolemPartBlock() -> void
{
    const char* golem_parts[] = { "Head", "Left arm", "Left arm lower", "Flower"};

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

auto UserInterface::sectionSeparator() -> void
{
    ImGui::Dummy(ImVec2(0, s_section_padding));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, s_section_padding));
}

auto UserInterface::updatePartIndex() -> void
{
    // const char* golem_parts[] = { "Head", "Left arm", "Left arm lower", "Flower"};
    //                                17         75           90              101
    switch (m_selected_golem_part)
    {
        case 0:
            m_selected_golem_part_model_index = 17;
            break;
        case 1:
            m_selected_golem_part_model_index = 75;
            break;
        case 2:
            m_selected_golem_part_model_index = 90;
            break;
        case 3:
            m_selected_golem_part_model_index = 101;
            break;
        default:
            m_selected_golem_part_model_index = 17;
            break;
    }
}

auto UserInterface::set() -> void
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

    ImGui::End();
}

auto UserInterface::render() -> void
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
