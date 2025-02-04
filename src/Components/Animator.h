//
// Created by Simon Cros on 29/01/2025.
//

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "Engine/FrameInfo.h"
#include "Engine/Engine.h"
#include "Engine/EngineComponent.h"

class Animator final : public EngineComponent
{
public:
    struct AnimatedTransform
    {
        std::optional<ft_glm::vec3> translation;
        std::optional<ft_glm::quat> rotation;
        std::optional<ft_glm::vec3> scale;
    };

private:
    bool m_animationChanged{false};
    int m_currentAnimationIndex{-1};
    DurationType m_timeSinceAnimationStart{DurationType::zero()};

    const Mesh& m_mesh;

    std::vector<AnimatedTransform> m_nodeTransforms;

public:
    explicit
    Animator(Object& object, const Mesh& mesh);

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
