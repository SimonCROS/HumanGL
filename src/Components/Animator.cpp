//
// Created by Simon Cros on 29/01/2025.
//

#include "Animator.h"

void Animator::onUpdate(Engine& engine)
{
    if (m_animationChanged)
    {
        m_timeSinceAnimationStart = DurationType::zero();
        m_animationChanged = false;
        std::fill(m_nodeTransforms.begin(), m_nodeTransforms.end(), AnimatedTransform{});
    }
    else
    {
        m_timeSinceAnimationStart += engine.frameInfo().deltaTime;
    }

    if (m_currentAnimationIndex < 0)
        return;

    const microgltf::Animation& gltfAnimation = m_mesh.model().animations[m_currentAnimationIndex];
    const Animation& animation = m_mesh.animations()[m_currentAnimationIndex];

    const float animationTime = fmodf(m_timeSinceAnimationStart.count(), animation.duration());

    for (const auto& channel : gltfAnimation.channels)
    {
        auto& nodeTransform = m_nodeTransforms[channel.target.node];
        switch (channel.target.path)
        {
        case microgltf::PathRotation:
            nodeTransform.rotation = animation.sampler(channel.sampler).quat(animationTime);
            break;
        case microgltf::PathScale:
            nodeTransform.scale = animation.sampler(channel.sampler).vec3(animationTime);
            break;
        case microgltf::PathTranslation:
            nodeTransform.translation = animation.sampler(channel.sampler).vec3(animationTime);
            break;
        default:
            break;
        }
    }
}
