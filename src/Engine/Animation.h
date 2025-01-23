//
// Created by Simon Cros on 1/23/25.
//

#ifndef ANIMATION_H
#define ANIMATION_H
#include <vector>

#include "AnimationSampler.h"

class Animation
{
public:
    struct AnimatedNode
    {
        int rotationSampler{-1};
        int scaleSampler{-1};
        int translationSampler{-1};
    };

private:
    float m_duration;
    std::vector<AnimationSampler> m_samplers;
    std::unordered_map<int, AnimatedNode> m_targetNodes;

public:
    Animation(const float duration, std::vector<AnimationSampler>&& samplers,
              std::unordered_map<int, AnimatedNode>&& targetNodes)
        : m_duration(duration), m_samplers(std::move(samplers)), m_targetNodes(targetNodes)
    {
    }

    static auto create(const microgltf::Model& model, const microgltf::Animation& animation) -> Animation;

    auto update(const FrameInfo& info) -> void;

    [[nodiscard]] auto animatedNode(const int node) const -> AnimatedNode
    {
        const auto it = m_targetNodes.find(node);
        if (it != m_targetNodes.end())
            return it->second;
        return {};
    }

    [[nodiscard]] auto sampler(int sampler) const -> AnimationSampler
    {
        return m_samplers[sampler];
    }
};

#endif //ANIMATION_H
