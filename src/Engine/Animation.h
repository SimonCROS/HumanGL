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
        std::unique_ptr<GLfloat[]> data;
    };

    struct OutputBuffer
    {
        size_t size;
        size_t attributeSize;
        size_t attributeByteStride;
        std::unique_ptr<GLubyte[]> data;
    };

private:
    float m_duration;
    std::unique_ptr<InputBuffer[]> m_inputBuffers;
    std::unique_ptr<OutputBuffer[]> m_outputBuffers;
    std::unordered_map<int, AnimatedNode> m_targetNodes;

    static auto initInputBuffer(const microgltf::Model& model, int accessorIndex) -> InputBuffer;
    static auto initOutputBuffer(const microgltf::Model& model, int accessorIndex) -> OutputBuffer;

public:
    Animation(const float duration, std::unique_ptr<InputBuffer[]>&& inputBuffers,
              std::unique_ptr<OutputBuffer[]>&& outputBuffers,
              std::unordered_map<int, AnimatedNode>&& targetNodes)
        : m_duration(duration), m_inputBuffers(std::move(inputBuffers)), m_outputBuffers(std::move(outputBuffers)),
          m_targetNodes(std::move(targetNodes))
    {
    }

    static auto Create(const microgltf::Model& model, const microgltf::Animation& animation) -> Animation;

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
