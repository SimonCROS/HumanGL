//
// Created by Simon Cros on 1/23/25.
//

#ifndef ANIMATION_H
#define ANIMATION_H
#include <vector>

#include "AnimationSampler.h"
#include "MicroGLTF/Struct.h"

class Animation
{
private:
    float m_duration;
    size_t m_samplerCount;
    std::vector<AnimationSampler> m_samplers;

    static auto initInputBuffer(const microgltf::Model& model, int accessorIndex) -> AnimationSampler::InputBuffer;
    static auto initOutputBuffer(const microgltf::Model& model, int accessorIndex) -> AnimationSampler::OutputBuffer;

public:
    Animation(const float duration,
              const size_t samplerCount,
              std::vector<AnimationSampler>&& samplers)
        : m_duration(duration),
          m_samplerCount(samplerCount),
          m_samplers(samplers)
    {
    }

    static auto Create(const microgltf::Model& model, const microgltf::Animation& animation) -> Animation;

    [[nodiscard]] auto duration() const -> float { return m_duration; }

    [[nodiscard]] auto sampler(const size_t index) const -> const AnimationSampler& { return m_samplers[index]; }
    [[nodiscard]] auto samplersCount() const -> size_t { return m_samplerCount; }
};

#endif //ANIMATION_H
