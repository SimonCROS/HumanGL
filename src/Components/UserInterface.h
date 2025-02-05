//
// Created by loumarti on 1/25/25.
//

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "Engine/Engine.h"
#include "Engine/EngineComponent.h"

class Animator;
class MeshRenderer;

class UserInterface final : public EngineComponent
{
private:
    static constexpr int s_frame_x = 8;
    static constexpr int s_frame_y = 8;
    static constexpr int s_frame_width = 230;
    static constexpr int s_frame_height = 420;
    static constexpr float s_text_offset = 100.0f;
    static constexpr float s_section_padding = 8.0f;

    Animator *m_animator; // Should not be a pointer
    MeshRenderer *m_meshRenderer; // Should not be a pointer

    std::vector<const char *> m_animationsNames;

    int m_selected_part{0};
    int m_selected_index{0};

    bool m_fill_mode = true;

public:
    explicit UserInterface(Object& object, const Window& window);
    UserInterface(const UserInterface& other) = delete;
    ~UserInterface() override;

    auto operator=(const UserInterface& other) -> UserInterface& = delete;

    auto onUpdate(Engine& engine) -> void override;
    auto onPostRender(Engine& engine) -> void override;

private:
    auto newFrame() const -> void;
    auto setAnimationBlock() -> void;
    auto setGolemPartBlock() -> void;
    auto setDisplayModeBlock() -> void;
    auto addSectionSeparator() const -> void;
};


#endif //USERINTERFACE_H
