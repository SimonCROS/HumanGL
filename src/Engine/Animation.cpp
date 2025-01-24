//
// Created by Simon Cros on 1/23/25.
//

#include "Animation.h"

auto Animation::create(const microgltf::Model& model, const microgltf::Animation& animation) -> Animation
{
    float duration = 0;
    std::vector<AnimationSampler> samplers;
    std::unordered_map<int, AnimatedNode> targetNodes;

    samplers.reserve(animation.samplers.size());
    targetNodes.reserve(animation.channels.size());

    for (const auto& animationSampler : animation.samplers)
    {
        auto& sampler = samplers.emplace_back(model, animationSampler);
        duration = std::max(duration, sampler.duration());
    }

    for (const auto& animationChannel : animation.channels)
    {
        auto& animatedNode = targetNodes.try_emplace(animationChannel.target.node).first->second;
        switch (animationChannel.target.path)
        {
        case microgltf::PathRotation:
            animatedNode.rotationSampler = animationChannel.sampler;
            break;
        case microgltf::PathScale:
            animatedNode.scaleSampler = animationChannel.sampler;
            break;
        case microgltf::PathTranslation:
            animatedNode.translationSampler = animationChannel.sampler;
            break;
        default:
            break;
        }
    }

    return {duration, std::move(samplers), std::move(targetNodes)};
}

auto Animation::update(const FrameInfo& info) -> void
{
    const float animationTime = fmodf(info.time.count(), m_duration);

    for (auto& sampler : m_samplers)
        sampler.update(animationTime);
}
