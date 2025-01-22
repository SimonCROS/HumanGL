//
// Created by Simon Cros on 22/01/2025.
//

#ifndef ANIMATIONSAMPLER_H
#define ANIMATIONSAMPLER_H
#include "Engine.h"
#include "MicroGLTF/Model.h"

class AnimationSampler {
private:
    const size_t m_accessorIndex; // TODO set

    uint64_t m_currentFrame{std::numeric_limits<uint64_t>::max()};
    uint32_t m_inputIndex{0};

public:
    auto update(const microgltf::Model& model, const FrameInfo& frameInfo) -> void
    {
        if (frameInfo.frameCount == m_currentFrame)
            return;

        const auto& accessor = model.accessors[m_accessorIndex]; // TODO cache ?
        const auto& bufferView = model.bufferViews[accessor.bufferView]; // TODO cache ?
        const auto& buffer = model.buffers[bufferView.buffer]; // TODO cache ?

        // TODO interpolation depending on frame info inside the buffer
    }
};

#endif //ANIMATIONSAMPLER_H
