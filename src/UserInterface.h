//
// Created by loumarti on 1/25/25.
//

#ifndef USERINTERFACE_H
#define USERINTERFACE_H


#include <string>
#include <unordered_map>

#include "imgui_impl_glfw.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



class UserInterface
{
private:
    int m_selected_animation;

    static constexpr int s_frame_x = 8;
    static constexpr int s_frame_y = 8;
    static constexpr int s_frame_width = 260;
    static constexpr int s_frame_height = 260;
    static constexpr float s_text_offset = 100.0f;
    static constexpr float s_section_padding = 8.0f;
    static constexpr int s_animation_max_index = 10;

public:
    explicit UserInterface(GLFWwindow *window);
    ~UserInterface();

    UserInterface(const UserInterface& other) = delete;
    UserInterface& operator=(const UserInterface& other) = delete;

    UserInterface(UserInterface&& other) noexcept
        : m_selected_animation(other.m_selected_animation)
    {
    }

    UserInterface& operator=(UserInterface&& other) noexcept
    {
        if (this == &other)
            return *this;
        m_selected_animation = other.m_selected_animation;
        return *this;
    }

    [[nodiscard]] int selected_animation() const
    {
        return m_selected_animation;
    }


    void set();

    void render();

    static std::string get_golem_animation_name(int index);

private:
    void newFrame();
    void setAnimationBlock();
    void sectionSeparator();
};


#endif //USERINTERFACE_H
