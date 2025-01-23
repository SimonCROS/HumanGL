//
// Created by Simon Cros on 22/01/2025.
//

#ifndef ANIMATIONSAMPLER_H
#define ANIMATIONSAMPLER_H
#include "Engine.h"
#include "glm/gtc/type_ptr.hpp"
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
        size_t prevIndex;
        size_t nextIndex;
        float t;
    };

    auto initInput(const microgltf::Model& model, int inputAccessorIndex) -> void;
    auto initOutput(const microgltf::Model& model, int outputAccessorIndex) -> void;

    [[nodiscard]] auto getInput(float time) const -> InputResult;

public:
    AnimationSampler(const microgltf::Model& model, const microgltf::AnimationSampler& sampler);
    // AnimationSampler(const AnimationSampler&) = delete;
    AnimationSampler(const AnimationSampler&) = default; // TODO remove, tmp
    AnimationSampler(AnimationSampler&& other) noexcept;
    ~AnimationSampler() = default;

    auto operator=(const AnimationSampler&) -> AnimationSampler& = delete;
    auto operator=(AnimationSampler&& other) noexcept -> AnimationSampler&;

    [[nodiscard]] auto interpolation() const -> microgltf::AnimationSamplerInterpolation
    {
        return m_interpolation;
    }

    [[nodiscard]] auto duration() const -> float
    {
        return m_inputMax;
    }

    [[nodiscard]] auto vec3() const -> glm::vec3
    {
        assert(m_prevValuePtr != nullptr && "You need to call update at lease once before using current");
        return glm::mix(*static_cast<glm::vec3*>(m_prevValuePtr),
                        *static_cast<glm::vec3*>(m_nextValuePtr),
                        m_interpolationTime);
    }

    [[nodiscard]] auto vec4() const -> glm::vec4
    {
        assert(m_prevValuePtr != nullptr && "You need to call update at lease once before using current");
        return glm::mix(*static_cast<glm::vec4*>(m_prevValuePtr),
                        *static_cast<glm::vec4*>(m_nextValuePtr),
                        m_interpolationTime);
    }

    [[nodiscard]] auto quat() const -> glm::quat
    {
        assert(m_prevValuePtr != nullptr && "You need to call update at lease once before using current");
        const float* prev = static_cast<float*>(m_prevValuePtr);
        const float* next = static_cast<float*>(m_nextValuePtr);
        return glm::slerp(glm::quat(prev[3], prev[0], prev[1], prev[2]),
                          glm::quat(next[3], next[0], next[1], next[2]),
                          m_interpolationTime);
    }

    auto update(float time) -> void;
};

#endif //ANIMATIONSAMPLER_H
