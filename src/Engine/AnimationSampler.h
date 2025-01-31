//
// Created by Simon Cros on 22/01/2025.
//

#ifndef ANIMATIONSAMPLER_H
#define ANIMATIONSAMPLER_H
#include "Engine.h"
#include "glm/gtc/type_ptr.hpp"
#include "MicroGLTF/Struct.h"

class AnimationSampler
{
private:
    Animation::SamplerData m_samplerData;

    void* m_prevValuePtr{nullptr};
    void* m_nextValuePtr{nullptr};
    float m_interpolationTime{};

    struct InputResult
    {
        size_t prevIndex;
        size_t nextIndex;
        float t;
    };

    [[nodiscard]] auto getInput(float time) const -> InputResult;

public:
    explicit AnimationSampler(const Animation::SamplerData& samplerData);

    [[nodiscard]] auto interpolation() const -> microgltf::AnimationSamplerInterpolation { return m_samplerData.interpolation; }
    [[nodiscard]] auto duration() const -> float { return m_samplerData.input.max; }

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
