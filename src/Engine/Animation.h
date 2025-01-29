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

    struct InputBuffer
    {
        size_t size;
        GLfloat max;
        GLfloat* data;
    };

    struct OutputBuffer
    {
        size_t size;
        size_t attributeSize;
        size_t attributeByteStride;
        GLubyte* data;
    };

private:
    struct InternalInputBuffer
    {
        size_t size;
        GLfloat max;
        std::unique_ptr<GLfloat[]> data;

        // ReSharper disable once CppNonExplicitConversionOperator
        // NOLINTNEXTLINE(*-explicit-constructor)
        constexpr operator InputBuffer() const { return {size, max, data.get()}; }
    };

    struct InternalOutputBuffer
    {
        size_t size;
        size_t attributeSize;
        size_t attributeByteStride;
        std::unique_ptr<GLubyte[]> data;

        // ReSharper disable once CppNonExplicitConversionOperator
        // NOLINTNEXTLINE(*-explicit-constructor)
        constexpr operator OutputBuffer() const { return {size, attributeSize, attributeByteStride, data.get()}; }
    };

    float m_duration;
    // TODO store interpolation and buffers in same vector
    std::unique_ptr<microgltf::AnimationSamplerInterpolation[]> m_interpolations;
    std::unique_ptr<InternalInputBuffer[]> m_inputBuffers;
    std::unique_ptr<InternalOutputBuffer[]> m_outputBuffers;
    std::unordered_map<int, AnimatedNode> m_targetNodes;

    static auto initInputBuffer(const microgltf::Model& model, int accessorIndex) -> InternalInputBuffer;
    static auto initOutputBuffer(const microgltf::Model& model, int accessorIndex) -> InternalOutputBuffer;

public:
    Animation(const float duration,
              std::unique_ptr<microgltf::AnimationSamplerInterpolation[]>&& interpolations,
              std::unique_ptr<InternalInputBuffer[]>&& inputBuffers,
              std::unique_ptr<InternalOutputBuffer[]>&& outputBuffers,
              std::unordered_map<int, AnimatedNode>&& targetNodes)
        : m_duration(duration),
          m_interpolations(std::move(interpolations)),
          m_inputBuffers(std::move(inputBuffers)),
          m_outputBuffers(std::move(outputBuffers)),
          m_targetNodes(std::move(targetNodes))
    {
    }

    static auto Create(const microgltf::Model& model, const microgltf::Animation& animation) -> Animation;

    [[nodiscard]] auto animatedNode(const int node) const -> AnimatedNode
    {
        const auto it = m_targetNodes.find(node);
        if (it != m_targetNodes.end())
            return it->second;
        return {};
    }

    [[nodiscard]] auto duration() const -> float { return m_duration; }

    [[nodiscard]] auto interpolation(const size_t index) const -> microgltf::AnimationSamplerInterpolation { return m_interpolations[index]; }
    [[nodiscard]] auto inputBuffer(const size_t index) const -> InputBuffer { return m_inputBuffers[index]; }
    [[nodiscard]] auto outputBuffer(const size_t index) const -> OutputBuffer { return m_outputBuffers[index]; }
};

#endif //ANIMATION_H
