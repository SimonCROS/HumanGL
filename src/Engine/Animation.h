//
// Created by Simon Cros on 1/23/25.
//

#ifndef ANIMATION_H
#define ANIMATION_H
#include <vector>

#include "AnimationSampler.h"

class Animation
{
private:
    float m_duration;
    std::vector<AnimationSampler> m_samplers;
    std::unordered_map<microgltf::AnimationChannelTarget, int, microgltf::AnimationChannelTargetHash> m_targetNodes;

public:
    Animation(const float duration, std::vector<AnimationSampler>&& samplers,
              std::unordered_map<microgltf::AnimationChannelTarget, int, microgltf::AnimationChannelTargetHash>&&
              targetNodes)
        : m_duration(duration), m_samplers(std::move(samplers)), m_targetNodes(targetNodes)
    {
    }

    static auto create(const microgltf::Model& model, const microgltf::Animation& animation) -> Animation;

    auto update(const FrameInfo& info) -> void;

    [[nodiscard]] auto tryGetSampledVec3Value(const microgltf::AnimationChannelTarget target) const -> std::optional<glm::vec3>
    {
        const auto it = m_targetNodes.find(target);
        if (it == m_targetNodes.end())
            return std::nullopt;
        else
            return m_samplers[it->second].vec3();
    }

    [[nodiscard]] auto tryGetSampledVec4Value(const microgltf::AnimationChannelTarget target) const -> std::optional<glm::vec4>
    {
        const auto it = m_targetNodes.find(target);
        if (it == m_targetNodes.end())
            return std::nullopt;
        else
            return m_samplers[it->second].vec4();
    }

    [[nodiscard]] auto tryGetSampledQuatValue(const microgltf::AnimationChannelTarget target) const -> std::optional<glm::quat>
    {
        const auto it = m_targetNodes.find(target);
        if (it == m_targetNodes.end())
            return std::nullopt;
        else
            return m_samplers[it->second].quat();
    }
};

#endif //ANIMATION_H
