//
// Created by loumarti on 1/25/25.
//

#include "UserInterface.h"


UserInterface::UserInterface(GLFWwindow* window) : m_selected_animation(0)
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

void UserInterface::newFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void UserInterface::setAnimationBlock()
{
    ImGui::Text("Select animation");
    if (ImGui::InputInt("##", &m_selected_animation, 1, 5)) {
        if (m_selected_animation < 0)
            m_selected_animation = 0;
        if (m_selected_animation > s_animation_max_index)
            m_selected_animation = s_animation_max_index;
    }
    ImGui::Dummy(ImVec2(s_text_offset, 0));
    ImGui::SameLine();
    ImGui::Text("%s", get_golem_animation_name(m_selected_animation).c_str());
}

void UserInterface::sectionSeparator()
{

    ImGui::Dummy(ImVec2(0, s_section_padding));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, s_section_padding));
}

void UserInterface::set()
{
    newFrame();
    constexpr auto windowSize = ImVec2(s_frame_width, s_frame_height);
    constexpr auto windowPos = ImVec2(s_frame_x, s_frame_y);
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);

    ImGui::Begin("GolemGL");
    setAnimationBlock();
    sectionSeparator();

    ImGui::End();
}

void UserInterface::render()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

std::string UserInterface::get_golem_animation_name(int index)
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
