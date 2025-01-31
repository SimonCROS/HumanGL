//
// Created by Simon Cros on 1/23/25.
//

#ifndef ANIMATION_H
#define ANIMATION_H
#include <unordered_map>
#include <vector>

#include "glad/gl.h"
#include "MicroGLTF/Struct.h"

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

    struct SamplerData
    {
        InputBuffer input;
        OutputBuffer output;
        microgltf::AnimationSamplerInterpolation interpolation;
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

    struct InternalSamplerData
    {
        InternalInputBuffer input;
        InternalOutputBuffer output;
        microgltf::AnimationSamplerInterpolation interpolation;

        // ReSharper disable once CppNonExplicitConversionOperator
        // NOLINTNEXTLINE(*-explicit-constructor)
        constexpr operator SamplerData() const { return {input, output, interpolation}; }
    };

    float m_duration;
    size_t m_samplerCount;
    std::vector<InternalSamplerData> m_samplerData;
    std::unordered_map<int, AnimatedNode> m_targetNodes;

    static auto initInputBuffer(const microgltf::Model& model, int accessorIndex) -> InternalInputBuffer;
    static auto initOutputBuffer(const microgltf::Model& model, int accessorIndex) -> InternalOutputBuffer;

public:
    Animation(const float duration,
              const size_t samplerCount,
              std::vector<InternalSamplerData>&& samplerData,
              std::unordered_map<int, AnimatedNode>&& targetNodes)
        : m_duration(duration),
          m_samplerCount(samplerCount),
          m_samplerData(std::move(samplerData)),
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

    [[nodiscard]] auto samplerData(const size_t index) const -> SamplerData { return m_samplerData[index]; }
    [[nodiscard]] auto samplersCount() const -> size_t { return m_samplerCount; }
};

#endif //ANIMATION_H
