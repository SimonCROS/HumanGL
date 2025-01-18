//
// Created by Simon Cros on 18/01/2025.
//

#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <vector>

#include "Components/Component.h"
#include "Engine/Transform.h"

class Actor
{
private:
    Actor* m_parent = nullptr;
    std::vector<Actor*> m_children;

    Transform m_transform;

    std::vector<Component*> m_components;

public:
    [[nodiscard]] auto parent() const -> Actor* { return m_parent; }
    [[nodiscard]] auto children() const -> const std::vector<Actor*>& { return m_children; }

    auto setParent(Actor* parent) -> void;
    auto addChild(Actor* child) -> void;
    auto removeChild(Actor* child) -> void;
    [[nodiscard]] auto hasChild(const Actor* child) const -> bool;

    auto addComponent(Component* component) -> void;
    auto removeComponent(Component* component) -> void;

private:
    auto appendChild(Actor* child) -> bool;
    auto eraseChild(Actor* child) -> bool;
    auto findChild(const Actor* child) -> std::vector<Actor*>::iterator;
    [[nodiscard]] auto findChild(const Actor* child) const -> std::vector<Actor*>::const_iterator;

    auto findComponent(const Component* child) -> std::vector<Component*>::iterator;
    [[nodiscard]] auto findComponent(const Component* child) const -> std::vector<Component*>::const_iterator;
};

#endif //ACTOR_HPP
