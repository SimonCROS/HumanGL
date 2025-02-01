//
// Created by Simon Cros on 29/01/2025.
//

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Transform.h"
#include "../Engine/FrameInfo.h"
#include "../Engine/Engine.h"

class Animator final : public EngineComponent
{
public:
    struct AnimatedTransform
    {
        std::optional<glm::vec3> translation;
        std::optional<glm::quat> rotation;
        std::optional<glm::vec3> scale;
    };

private:
    bool m_animationChanged{false};
    int m_currentAnimationIndex{-1};
    DurationType m_timeSinceAnimationStart{DurationType::zero()};

    const Mesh& m_mesh;

    std::vector<AnimatedTransform> m_nodeTransforms;

public:
    explicit
    Animator(Object& object, const Mesh& mesh) : EngineComponent(object), m_mesh(mesh)
    {
        m_nodeTransforms.resize(mesh.model().nodes.size());
    }

    auto onUpdate(Engine& engine) -> void override;

    auto setAnimation(const int index) -> void
    {
        m_currentAnimationIndex = index;
        m_animationChanged = true;
    }

    [[nodiscard]] auto nodeTransform(const int node) const -> const AnimatedTransform&
    {
        return m_nodeTransforms[node];
    }
};

#endif //ANIMATOR_H
