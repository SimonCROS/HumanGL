//
// Created by Simon Cros on 1/23/25.
//

#include "Animation.h"

auto Animation::create(const microgltf::Model& model, const microgltf::Animation& animation) -> Animation
{
    float duration = 0;
    std::vector<AnimationSampler> samplers;
    std::unordered_map<microgltf::AnimationChannelTarget, int, microgltf::AnimationChannelTargetHash> targetNodes;

    samplers.reserve(animation.samplers.size());
    targetNodes.reserve(animation.channels.size());

    for (const auto& animationSampler : animation.samplers)
    {
        auto& sampler = samplers.emplace_back(model, animationSampler);
        duration = std::max(duration, sampler.duration());
    }

    for (const auto& animationChannel : animation.channels)
    {
        auto result = targetNodes.try_emplace(animationChannel.target, animationChannel.sampler).second;
        assert(result);
    }

    return {duration, std::move(samplers), std::move(targetNodes)};
}

auto Animation::update(const FrameInfo& info) -> void
{
    const float animationTime = std::fmodf(info.time.count(), m_duration);

    for (auto& sampler : m_samplers)
        sampler.update(animationTime);
}
