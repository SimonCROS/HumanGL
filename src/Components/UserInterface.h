//
// Created by loumarti on 1/25/25.
//

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <string>

#include "Engine/Engine.h"
#include "Engine/EngineComponent.h"

class Animator;

class UserInterface final : public EngineComponent
{
private:
    int m_selected_animation;
    int m_selected_golem_part;
    int m_selected_golem_part_model_index;
    float m_scale_x = 1.0f;
    float m_scale_y = 1.0f;
    float m_scale_z = 1.0f;
    bool m_fill_mode = true;

    int m_custom_golem_part_model_index = 0;
    float m_custom_scale_x = 1.0f;
    float m_custom_scale_y = 1.0f;
    float m_custom_scale_z = 1.0f;

    static constexpr int s_frame_x = 8;
    static constexpr int s_frame_y = 8;
    static constexpr int s_frame_width = 230;
    static constexpr int s_frame_height = 500;
    static constexpr float s_text_offset = 100.0f;
    static constexpr float s_section_padding = 8.0f;
    static constexpr int s_golem_node_max_index = 145;

    Animator *m_animator; // Should not be a pointer

public:
    explicit UserInterface(Object& object, const Window& window);
    UserInterface(const UserInterface& other) = delete;
    ~UserInterface() override;

    auto operator=(const UserInterface& other) -> UserInterface& = delete;

    auto onUpdate(Engine& engine) -> void override;
    auto onPostRender(Engine& engine) -> void override;

    static auto get_golem_animation_name(int index) -> std::string;

    [[nodiscard]] auto selected_animation() const -> int
    {
        return m_selected_animation;
    }

    [[nodiscard]] auto scale_x() const -> float
    {
        return m_scale_x;
    }

    [[nodiscard]] auto scale_y() const -> float
    {
        return m_scale_y;
    }

    [[nodiscard]] auto scale_z() const -> float
    {
        return m_scale_z;
    }

    [[nodiscard]] auto custom_scale_x() const -> float
    {
        return m_custom_scale_x;
    }

    [[nodiscard]] auto custom_scale_y() const -> float
    {
        return m_custom_scale_y;
    }

    [[nodiscard]] auto custom_scale_z() const -> float
    {
        return m_custom_scale_z;
    }

    [[nodiscard]] auto selected_node() const -> int
    {
        return m_selected_golem_part_model_index;
    }

    [[nodiscard]] auto custom_node() const -> int
    {
        return m_custom_golem_part_model_index;
    }

private:
    auto newFrame() const -> void;
    auto setAnimationBlock() -> void;
    auto setGolemPartBlock() -> void;
    auto setCustomGolemPartBlock() -> void;
    auto setDisplayModeBlock() -> void;
    auto sectionSeparator() const -> void;
    auto updatePartIndex() -> void;
};


#endif //USERINTERFACE_H
