//
// Created by Simon Cros on 1/23/25.
//

#include "Animation.h"

auto Animation::initInputBuffer(const microgltf::Model& model, const int accessorIndex) -> InternalInputBuffer
{
    const auto& accessor = model.accessors[accessorIndex];
    assert(accessor.type == microgltf::Scalar);
    assert(accessor.componentType == GL_FLOAT);
    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];

    const size_t offset = bufferView.byteOffset + accessor.byteOffset;
    constexpr size_t attributeSize = sizeof(GLfloat); // accessor.componentType MUST be GL_FLOAT
    const size_t byteStride = microgltf::byteStride(bufferView, attributeSize);

    const size_t length = byteStride * accessor.count;
    assert(length > 0);
    assert(accessor.byteOffset + length <= bufferView.byteLength);

    const uint8_t* bytes = buffer.data.data() + offset;
    auto data = std::make_unique<GLfloat[]>(accessor.count);

    if (attributeSize == byteStride) // TODO disable optimization to test other branch
    {
        // if data is tightly packed, we can just copy the buffer
        std::copy_n(reinterpret_cast<const GLfloat*>(bytes), accessor.count, data.get());
    }
    else
    {
        for (size_t i = 0; i < accessor.count; ++i)
            data[i] = *reinterpret_cast<const GLfloat*>(bytes + (i * byteStride));
    }

    return {
        .size = accessor.count,
        .max = data[accessor.count - 1],
        .data = std::move(data),
    };
}

auto Animation::initOutputBuffer(const microgltf::Model& model, const int outputAccessorIndex) -> InternalOutputBuffer
{
    const auto& accessor = model.accessors[outputAccessorIndex];
    const auto& bufferView = model.bufferViews[accessor.bufferView];
    const auto& buffer = model.buffers[bufferView.buffer];

    const size_t offset = bufferView.byteOffset + accessor.byteOffset;
    const size_t attributeSize = microgltf::attributeSize(accessor);
    const size_t byteStride = microgltf::byteStride(bufferView, attributeSize);

    const size_t length = byteStride * accessor.count;
    assert(length > 0);
    assert(accessor.byteOffset + length <= bufferView.byteLength);

    const GLubyte* bytes = buffer.data.data() + offset;

    auto data = std::make_unique<GLubyte[]>(length);
    std::copy_n(bytes, length, data.get());

    return {
        .size = length,
        .attributeSize = attributeSize,
        .attributeByteStride = byteStride,
        .data = std::move(data),
    };
}

auto Animation::Create(const microgltf::Model& model, const microgltf::Animation& animation) -> Animation
{
    float duration = 0;
    std::unordered_map<int, AnimatedNode> targetNodes;
    const auto samplerCount = model.samplers.size();
    auto samplerData = std::vector<InternalSamplerData>();

    samplerData.reserve(samplerCount);
    targetNodes.reserve(animation.channels.size());
    for (size_t i = 0; i < animation.samplers.size(); ++i)
    {
        auto input = initInputBuffer(model, animation.samplers[i].input);
        auto output = initOutputBuffer(model, animation.samplers[i].output);

        duration = std::max(duration, input.max);
        samplerData.emplace_back(
            std::move(input),
            std::move(output),
            animation.samplers[i].interpolation
        );
    }

    for (const auto& animationChannel : animation.channels)
    {
        auto& animatedNode = targetNodes.try_emplace(animationChannel.target.node).first->second;
        switch (animationChannel.target.path)
        {
        case microgltf::PathRotation:
            animatedNode.rotationSampler = animationChannel.sampler;
            break;
        case microgltf::PathScale:
            animatedNode.scaleSampler = animationChannel.sampler;
            break;
        case microgltf::PathTranslation:
            animatedNode.translationSampler = animationChannel.sampler;
            break;
        default:
            break;
        }
    }

    return {
        duration, samplerCount, std::move(samplerData), std::move(targetNodes)
    };
}
