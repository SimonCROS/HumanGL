//
// Created by loumarti on 2/7/25.
//

#include "GolemUserInterface.h"
#include "Animator.h"
#include "MeshRenderer.h"
#include "UserInterface.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Engine/Object.h"

constexpr const char* parts[] = {
    "All",
    "Head",
    "Left arm", "Left arm lower", "Flower", "Left hand",
    "Right arm", "Right arm lower", "Right hand",
    "Left leg", "Left leg lower", "Left feet",
    "Right leg", "Right leg lower", "Right feet",
    "Custom",
};
constexpr int partToIndex[] = {
    1,
    17,
    75, 90, 101, 95,
    28, 48, 63,
    107, 116, 121,
    128, 135, 138,
    0,
};

constexpr int customPartIndex = IM_ARRAYSIZE(parts) - 1;

GolemUserInterface::GolemUserInterface(Object& object, const Window& window, ImguiWindowData windowData)
    : UserInterface(object, window, "Golem", windowData)
{
    m_animator = &object.getComponent<Animator>()->get();

    m_animationsNames.reserve(m_animator->mesh().model().animations.size() + 1);
    m_animationsNames.push_back("-");
    for (const auto& animation : m_animator->mesh().model().animations)
        m_animationsNames.push_back(animation.name.c_str());

    m_selectedIndex = partToIndex[m_selectedPart];
}

auto GolemUserInterface::onUpdate(Engine& engine) -> void
{
    constexpr auto windowSize = ImVec2(static_cast<float>(s_windowData.s_frame_width), static_cast<float>(s_windowData.s_frame_height));
    constexpr auto windowPos = ImVec2(static_cast<float>(s_windowData.s_frame_x), static_cast<float>(s_windowData.s_frame_y));
    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Once);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Once);

    ImGui::Begin("GolemGL");
    setAnimationBlock();
    addSectionSeparator();
    setGolemPartBlock();
    addSectionSeparator();
    setDisplayModeBlock(engine);
    addSectionSeparator();
    setDisplayBlock();

    ImGui::End();
}

auto GolemUserInterface::setAnimationBlock() const -> void
{
    // Add 1 because index -1 is 0 for imgui
    int selectedIndex = m_animator->currentAnimationIndex() + 1;

    ImGui::Text("Select animation");

    if (ImGui::Combo("#0", &selectedIndex, m_animationsNames.data(), static_cast<int>(m_animationsNames.size())))
        m_animator->setAnimation(selectedIndex - 1);

    ImGui::Dummy(ImVec2(s_text_offset, 0));
}

auto GolemUserInterface::setGolemPartBlock() -> void
{
    ImGui::Text("Select golem part");
    if (ImGui::Combo("#1", &m_selectedPart, parts, IM_ARRAYSIZE(parts)))
        m_selectedIndex = partToIndex[m_selectedPart];

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;
    if (m_selectedPart != customPartIndex)
        flags |= ImGuiInputTextFlags_ReadOnly;

    if (ImGui::InputInt("#2", &m_selectedIndex, 1, 10, flags))
        m_selectedIndex = std::clamp(m_selectedIndex, 0,
                                     static_cast<int>(m_meshRenderer->mesh().model().nodes.size()));

    ImGui::Text("Customize scale");

    ft_glm::vec3 scaleMultiplier = m_meshRenderer->getScaleMultiplier(m_selectedIndex);
    if (ImGui::InputFloat("x", &scaleMultiplier.x, 0.1f, 0.5f, "%.1f"))
    {
        if (scaleMultiplier.x < 0.0f)
            scaleMultiplier.x = 0.0f;
    }
    if (ImGui::InputFloat("y", &scaleMultiplier.y, 0.1f, 0.5f, "%.1f"))
    {
        if (scaleMultiplier.y < 0.0f)
            scaleMultiplier.y = 0.0f;
    }
    if (ImGui::InputFloat("z", &scaleMultiplier.z, 0.1f, 0.5f, "%.1f"))
    {
        if (scaleMultiplier.z < 0.0f)
            scaleMultiplier.z = 0.0f;
    }

    m_meshRenderer->setScaleMultiplier(m_selectedIndex, scaleMultiplier);
}
