//
// Created by loumarti on 1/25/25.
//

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <string>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Engine.h"

class UserInterface
{
private:
    int m_selected_animation;
    int m_selected_golem_part;
    int m_selected_golem_part_model_index;
    float m_scale_x = 1.0f;
    float m_scale_y = 1.0f;
    float m_scale_z = 1.0f;
    bool m_fill_mode = true;

    bool m_jumping = false;
    float m_jump_height = s_jump_height_default;
    float m_jump_duration = s_jump_duration_default;

    int m_custom_golem_part_model_index = 0;
    float m_custom_scale_x = 1.0f;
    float m_custom_scale_y = 1.0f;
    float m_custom_scale_z = 1.0f;

    static constexpr int s_frame_x = 8;
    static constexpr int s_frame_y = 8;
    static constexpr int s_frame_width = 230;
    static constexpr int s_frame_height = 560;
    static constexpr float s_text_offset = 100.0f;
    static constexpr float s_section_padding = 8.0f;
    static constexpr int s_animation_max_index = 10;
    static constexpr int s_golem_node_max_index = 145;
    static constexpr float s_jump_height_default = 1.0f;
    static constexpr float s_jump_duration_default = 0.8f;

public:
    explicit UserInterface(Engine& engine);
    UserInterface(const UserInterface& other) = delete;
    ~UserInterface();

    auto operator=(const UserInterface& other) -> UserInterface& = delete;

    auto set() -> void;
    auto render() -> void;
    [[nodiscard]] auto computeJumpZ(float time) const -> float;

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

    [[nodiscard]] auto jumping() const -> int
    {
        return m_jumping;
    }

private:
    auto newFrame() const -> void;
    auto setAnimationBlock() -> void;
    auto setGolemPartBlock() -> void;
    auto setCustomGolemPartBlock() -> void;
    auto setDisplayModeBlock() -> void;
    auto setJumpingBlock() -> void;
    auto sectionSeparator() const -> void;
    auto updatePartIndex() -> void;
};


#endif //USERINTERFACE_H
