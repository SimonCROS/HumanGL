//
// Created by Simon Cros on 1/17/25.
//

#include <algorithm>

#include "Mesh.h"

auto Mesh::computeTransformMatrix() const -> glm::mat4
{
    constexpr auto identity = glm::identity<glm::mat4>();
    const auto rotate = glm::translate(identity, m_pivot * -1.0f)
        * glm::mat4_cast(m_transform.rotation())
        * glm::translate(identity, m_pivot);

    return rotate
        * glm::scale(identity, m_transform.scale())
        * glm::translate(identity, m_transform.position() - (m_pivot * m_transform.scale()));
}


auto Mesh::setParent(Mesh* parent) -> void
{
    if (parent == m_parent)
        return;

    if (m_parent != nullptr)
        m_parent->eraseChild(this);

    if (parent != nullptr)
        parent->appendChild(this);

    m_parent = parent;
}

auto Mesh::addChild(Mesh* child) -> void
{
    assert(child != nullptr);
    child->setParent(this);
}

auto Mesh::removeChild(Mesh* child) -> void
{
    assert(child != nullptr);
    if (eraseChild(child))
        child->m_parent = nullptr;
}

auto Mesh::hasChild(const Mesh* child) const -> bool
{
    return findChild(child) != m_children.end();
}

auto Mesh::appendChild(Mesh* child) -> bool
{
    if (hasChild(child))
        return false;
    m_children.emplace_back(child);
    return true;
}

auto Mesh::eraseChild(Mesh* child) -> bool
{
    const auto it = findChild(child);
    if (it == m_children.end())
        return false;
    m_children.erase(it);
    return true;
}

auto Mesh::findChild(const Mesh* child) -> std::vector<Mesh*>::iterator
{
    // If ranges is available
    // return std::ranges::find(m_children, child);
    return std::find(m_children.begin(), m_children.end(), child);
}

auto Mesh::findChild(const Mesh* child) const -> std::vector<Mesh*>::const_iterator
{
    // If ranges is available
    // return std::ranges::find(m_children, child);
    return std::find(m_children.cbegin(), m_children.cend(), child);
}