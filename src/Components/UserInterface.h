//
// Created by loumarti on 1/25/25.
//

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "GolemUserInterface.h"
#include "Engine/Engine.h"
#include "Engine/EngineComponent.h"

class Animator;
class MeshRenderer;

struct ImguiWindowData
{
    int s_frame_x = 8;
    int s_frame_y = 8;
    int s_frame_width = 230;
    int s_frame_height = 140;
};

class UserInterface : public EngineComponent
{
protected:
    static constexpr float s_text_offset = 100.0f;
    static constexpr float s_section_padding = 8.0f;
    inline static int s_instanceCount = 0;
    inline static bool s_renderCalled = false;
    inline static bool s_newFrameCalled = false;

    std::string m_name;
    ImguiWindowData m_windowData;

    static std::size_t s_nextID;


    // Animator *m_animator; // Should not be a pointer
    MeshRenderer *m_meshRenderer; // Should not be a pointer

    // std::vector<const char *> m_animationsNames;
    //
    // int m_selectedPart{0};
    // int m_selectedIndex{0};
    std::size_t m_id;
    int m_selectedDisplayMode{0};
    bool m_display{true};

public:
    explicit UserInterface(Object& object, const Window& window, std::string_view name = "default interface", const ImguiWindowData& windowData = {});
    UserInterface(const UserInterface& other) = delete;
    ~UserInterface() override;

    auto operator=(const UserInterface& other) -> UserInterface& = delete;

    auto initializeImGui(const Window& window) -> void;
    auto onUpdate(Engine& engine) -> void override;
    auto onPostRender(Engine& engine) -> void override;

protected:
    auto newFrame() const -> void;
    // auto setAnimationBlock() -> void;
    // auto setGolemPartBlock() -> void;
    auto setDisplayModeBlock(Engine& engine) -> void;
    auto setDisplayBlock(Engine& engine) -> void;
    auto addSectionSeparator() const -> void;
    auto makeLabel(std::string_view prefix) const -> std::string;

    static auto generateUniqueID() -> std::size_t {
        return s_nextID++;
    }
};



#endif //USERINTERFACE_H
