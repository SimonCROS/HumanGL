//
// Created by loumarti on 1/25/25.
//

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include <string>
#include <unordered_map>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

class UserInterface
{
private:
    int m_selected_animation;
    int m_selected_golem_part;
    int m_selected_golem_part_model_index;
    float m_scale_x = 1.0f;
    float m_scale_y = 1.0f;
    float m_scale_z = 1.0f;

    static constexpr int s_frame_x = 8;
    static constexpr int s_frame_y = 8;
    static constexpr int s_frame_width = 260;
    static constexpr int s_frame_height = 260;
    static constexpr float s_text_offset = 100.0f;
    static constexpr float s_section_padding = 8.0f;
    static constexpr int s_animation_max_index = 10;

public:
    explicit UserInterface(GLFWwindow *window);
    UserInterface(const UserInterface& other) = delete;
    ~UserInterface();

    auto operator=(const UserInterface& other) -> UserInterface& = delete;

    auto set() -> void;
    auto render() -> void;

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
        return m_scale_x;
    }

    [[nodiscard]] auto scale_z() const -> float
    {
        return m_scale_x;
    }

    [[nodiscard]] auto selected_node() const -> int
    {
        return m_selected_golem_part_model_index;
    }

private:
    auto newFrame() -> void;
    auto setAnimationBlock() -> void;
    auto setGolemPartBlock() -> void;
    auto sectionSeparator() -> void;
    auto updatePartIndex() -> void;
};


#endif //USERINTERFACE_H
