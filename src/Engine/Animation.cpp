//
// Created by Simon Cros on 1/23/25.
//

#include "Animation.h"

auto Animation::initInputBuffer(const microgltf::Model& model, const int accessorIndex) -> InputBuffer
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

auto Animation::initOutputBuffer(const microgltf::Model& model, const int outputAccessorIndex) -> OutputBuffer
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
    std::unique_ptr<InputBuffer[]> inputBuffers;
    std::unique_ptr<OutputBuffer[]> outputBuffers;
    std::unordered_map<int, AnimatedNode> targetNodes;

    inputBuffers = std::make_unique<InputBuffer[]>(animation.samplers.size());
    outputBuffers = std::make_unique<OutputBuffer[]>(animation.samplers.size());
    targetNodes.reserve(animation.channels.size());

    for (const auto& animationSampler : animation.samplers)
    {
        inputBuffers -
        auto& sampler = samplers.emplace_back(model, animationSampler);
        duration = std::max(duration, sampler.duration());
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

    return {duration, std::move(samplers), std::move(targetNodes)};
}

auto Animation::update(const FrameInfo& info) -> void
{
    const float animationTime = fmodf(info.time.count(), m_duration);

    for (auto& sampler : m_samplers)
        sampler.update(animationTime);
}
