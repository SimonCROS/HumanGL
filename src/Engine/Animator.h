//
// Created by Simon Cros on 29/01/2025.
//

#ifndef ANIMATOR_H
#define ANIMATOR_H
#include <cmath>

#include "Engine.h"
#include "AnimationSampler.h"

class Animator
{
private:
    bool m_animationChanged{false};
    int m_currentAnimationIndex{-1};
    DurationType m_timeSinceAnimationStart{DurationType::zero()};

    const std::vector<Animation>& m_animations;

    struct AnimationSamplers
    {
        std::vector<AnimationSampler> m_samplers;
    };

    std::vector<AnimationSamplers> m_animationsSamplers;

public:
    explicit
    Animator(const std::vector<Animation>& animations) : m_animations(animations)
    {
        m_animationsSamplers.reserve(m_animations.size());
        for (const auto & m_animation : m_animations)
        {
            auto samplers = m_animationsSamplers.emplace_back();
            samplers.m_samplers.reserve(m_animation.samplersCount());
            for (int i = 0; i < m_animation.samplersCount(); ++i)
            {
                samplers.m_samplers.emplace_back(m_animation.samplerData(i));
            }
        }
    }

    auto setAnimation(int index) -> void
    {
        m_currentAnimationIndex = index;
        m_animationChanged = true;
    }

    auto update(const FrameInfo& info) -> void
    {
        if (m_currentAnimationIndex < 0)
            return;

        const Animation& animation = m_animations[m_currentAnimationIndex];

        if (m_animationChanged)
        {
            m_timeSinceAnimationStart = DurationType::zero();
            m_animationChanged = false;
        }
        else
        {
            m_timeSinceAnimationStart += info.deltaTime;
        }

        const float animationTime = fmodf(m_timeSinceAnimationStart.count(), animation.duration());
        for (auto& sampler : m_animationsSamplers[m_currentAnimationIndex].m_samplers)
        {
            sampler.update(animationTime);
        }
    }
};

#endif //ANIMATOR_H
