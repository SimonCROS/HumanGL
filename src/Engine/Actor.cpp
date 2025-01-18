//
// Created by Simon Cros on 18/01/2025.
//

#include <algorithm>

#include "Actor.hpp"

auto Actor::setParent(Actor* parent) -> void
{
    if (parent == m_parent)
        return;

    if (m_parent != nullptr)
        m_parent->eraseChild(this);

    if (parent != nullptr)
        parent->appendChild(this);

    m_parent = parent;
}

auto Actor::addChild(Actor* child) -> void
{
    assert(child != nullptr);
    child->setParent(this);
}

auto Actor::removeChild(Actor* child) -> void
{
    assert(child != nullptr);
    if (eraseChild(child))
        child->m_parent = nullptr;
}

auto Actor::hasChild(const Actor* child) const -> bool
{
    return findChild(child) != m_children.end();
}

auto Actor::addComponent(Component* component) -> void
{
    assert(component != nullptr);
    m_components.emplace_back(component);
}

auto Actor::removeComponent(Component* component) -> void
{
    assert(component != nullptr);
    const auto it = findComponent(component);
    if (it != m_components.end())
        m_components.erase(it);
}

auto Actor::appendChild(Actor* child) -> bool
{
    if (hasChild(child))
        return false;
    m_children.emplace_back(child);
    return true;
}

auto Actor::eraseChild(Actor* child) -> bool
{
    const auto it = findChild(child);
    if (it == m_children.end())
        return false;
    m_children.erase(it);
    return true;
}

auto Actor::findChild(const Actor* child) -> std::vector<Actor*>::iterator
{
    // If ranges is available
    // return std::ranges::find(m_children, child);
    return std::find(m_children.begin(), m_children.end(), child);
}

auto Actor::findChild(const Actor* child) const -> std::vector<Actor*>::const_iterator
{
    // If ranges is available
    // return std::ranges::find(m_children, child);
    return std::find(m_children.cbegin(), m_children.cend(), child);
}

auto Actor::findComponent(const Component* child) -> std::vector<Component*>::iterator
{
    // If ranges is available
    // return std::ranges::find(m_components, child);
    return std::find(m_components.begin(), m_components.end(), child);
}

auto Actor::findComponent(const Component* child) const -> std::vector<Component*>::const_iterator
{
    // If ranges is available
    // return std::ranges::find(m_components, child);
    return std::find(m_components.cbegin(), m_components.cend(), child);
}
