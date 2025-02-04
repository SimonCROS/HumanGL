//
// Created by Simon Cros on 22/01/2025.
//

#ifndef ANIMATIONSAMPLER_H
#define ANIMATIONSAMPLER_H

#include "Engine.h"
#include "MicroGLTF/Struct.h"

class AnimationSampler
{
public:
    struct InputBuffer
    {
        size_t size;
        size_t attributeStride;
        size_t attributeSize;
        const GLfloat* data;
    };

    struct OutputBuffer
    {
        size_t size;
        size_t byteStride;
        size_t attributeSize;
        const GLubyte* data;
    };

private:
    GLfloat m_duration{};
    InputBuffer m_input;
    OutputBuffer m_output;

    struct InputResult
    {
        size_t prevIndex;
        size_t nextIndex;
        float t;
    };

    [[nodiscard]] auto getInput(float time) const -> InputResult;

    template<class T>
    [[nodiscard]] auto getOutputPtr(const size_t index) const -> const T*
    {
        return reinterpret_cast<const T*>(m_output.data + index * m_output.byteStride);
    }

public:
    AnimationSampler(const InputBuffer& input, const OutputBuffer& output);

    [[nodiscard]] auto duration() const -> float { return m_duration; }

    [[nodiscard]] auto vec3(const float time) const -> ft_glm::vec3
    {
        const auto result = getInput(time);
        return ft_glm::mix(*getOutputPtr<ft_glm::vec3>(result.prevIndex),
                        *getOutputPtr<ft_glm::vec3>(result.nextIndex),
                        result.t);
    }

    [[nodiscard]] auto vec4(const float time) const -> ft_glm::vec4
    {
        const auto result = getInput(time);
        return ft_glm::mix(*getOutputPtr<ft_glm::vec4>(result.prevIndex),
                        *getOutputPtr<ft_glm::vec4>(result.nextIndex),
                        result.t);
    }

    [[nodiscard]] auto quat(const float time) const -> ft_glm::quat
    {
        const auto result = getInput(time);
        const auto prev = getOutputPtr<float>(result.prevIndex);
        const auto next = getOutputPtr<float>(result.nextIndex);
        return ft_glm::slerp(ft_glm::quat(prev[3], prev[0], prev[1], prev[2]),
                          ft_glm::quat(next[3], next[0], next[1], next[2]),
                          result.t);
    }
};

#endif //ANIMATIONSAMPLER_H
