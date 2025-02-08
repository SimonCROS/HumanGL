//
// Created by loumarti on 2/7/25.
//

#ifndef GOLEMUSERINTERFACE_H
#define GOLEMUSERINTERFACE_H
#include "UserInterface.h"



class GolemUserInterface : public UserInterface {

private:
    static constexpr int s_frame_x = 8;
    static constexpr int s_frame_y = 320;
    static constexpr int s_frame_width = 230;
    static constexpr int s_frame_height = 290;

    Animator *m_animator; // Should not be a pointer
    std::vector<const char *> m_animationsNames;

    int m_selectedPart{0};
    int m_selectedIndex{0};

public:
    explicit GolemUserInterface(Object& object, const Window& window);
    GolemUserInterface(const GolemUserInterface& other) = delete;
    ~GolemUserInterface() override = default;

    auto operator=(const GolemUserInterface& other) -> GolemUserInterface& = delete;

private:
    auto onUpdate(Engine& engine) -> void override;
    auto setAnimationBlock() const -> void;
    auto setGolemPartBlock() -> void;

};



#endif //GOLEMUSERINTERFACE_H
