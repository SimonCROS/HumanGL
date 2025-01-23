//
// Created by Simon Cros on 22/01/2025.
//

#ifndef ANIMATIONSAMPLER_H
#define ANIMATIONSAMPLER_H
#include "Engine.h"
#include "MicroGLTF/Model.h"

class AnimationSampler
{
private:
    microgltf::AnimationSamplerInterpolation m_interpolation;

    GLfloat m_inputMax{};
    std::vector<GLfloat> m_inputBuffer;
    size_t m_outputAttributeSize{};
    size_t m_outputAttributeByteStride{};
    std::vector<GLubyte> m_outputBuffer;

    void* m_prevValuePtr{nullptr};
    void* m_nextValuePtr{nullptr};
    float m_interpolationTime{};
    uint64_t m_currentFrame{std::numeric_limits<uint64_t>::max()};

    struct InputResult
    {
        ptrdiff_t prevIndex;
        ptrdiff_t nextIndex;
        float t;
    };

    auto initInput(const microgltf::Model& model, int inputAccessorIndex) -> void;
    auto initOutput(const microgltf::Model& model, int outputAccessorIndex) -> void;

    [[nodiscard]] auto getInput(const FrameInfo& frameInfo) const -> InputResult;

public:
    AnimationSampler(const microgltf::Model& model, const microgltf::AnimationSampler& sampler);
    // AnimationSampler(const AnimationSampler&) = delete;
    AnimationSampler(const AnimationSampler&) = default; // TODO remove, tmp
    AnimationSampler(AnimationSampler&& other) noexcept;
    ~AnimationSampler() = default;

    auto operator=(const AnimationSampler&) -> AnimationSampler& = delete;
    auto operator=(AnimationSampler&& other) noexcept -> AnimationSampler&;

    template <class T, class Component>
    auto current(
        const std::function<T(microgltf::AnimationSamplerInterpolation, const Component*, const Component*, float)> interpolate) -> T
    {
        assert(m_prevValuePtr != nullptr && "You need to call update at lease once before using current");
        assert(m_nextValuePtr != nullptr && "You need to call update at lease once before using current");
        return interpolate(m_interpolation, static_cast<Component*>(m_prevValuePtr), static_cast<Component*>(m_nextValuePtr), m_interpolationTime);
    }

    auto update(const FrameInfo& frameInfo) -> void;
};

#endif //ANIMATIONSAMPLER_H
