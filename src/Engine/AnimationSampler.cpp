//
// Created by Simon Cros on 22/01/2025.
//

#include "AnimationSampler.h"

auto AnimationSampler::initInput(const microgltf::Model& model, const int inputAccessorIndex) -> void
{
    const auto& accessor = model.accessors[inputAccessorIndex];
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
    if (attributeSize == byteStride) // TODO disable optimization to test other branch
    {
        // if data is tightly packed, we can just assign everything using vector.assign
        m_inputBuffer.assign(reinterpret_cast<const GLfloat*>(bytes), reinterpret_cast<const GLfloat*>(bytes) + accessor.count);
    }
    else
    {
        m_inputBuffer.reserve(accessor.count);
        for (size_t i = 0; i < length; i += byteStride)
            m_inputBuffer.push_back(*reinterpret_cast<const GLfloat*>(bytes + i));
    }

    m_inputMax = m_inputBuffer.back();
}

auto AnimationSampler::initOutput(const microgltf::Model& model, const int outputAccessorIndex) -> void
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

    const uint8_t* bytes = buffer.data.data() + offset;

    m_outputBuffer.assign(bytes, bytes + length);
    m_outputAttributeByteStride = byteStride;
    m_outputAttributeSize = attributeSize;
}

auto AnimationSampler::getInput(const float time) const -> InputResult
{
    const auto upperBound = std::upper_bound(m_inputBuffer.begin(), m_inputBuffer.end(), time);
    // Clamp to first and last
    if (upperBound == m_inputBuffer.begin())
        return {0, 0, 0};
    if (upperBound == m_inputBuffer.end())
        return {m_inputBuffer.size() - 1, m_inputBuffer.size() - 1, 1};

    const auto next = upperBound;
    const auto prev = upperBound - 1;

    const auto prevVal = *prev;
    const auto nextVal = *next;
    const auto prevIndex = prev - m_inputBuffer.begin();
    const auto nextIndex = next - m_inputBuffer.begin();

    const float stepRatio = (nextVal != prevVal) ? ((time - prevVal) / (nextVal - prevVal)) : 0;
    return {prevIndex, nextIndex, stepRatio};
}

AnimationSampler::AnimationSampler(const microgltf::Model& model, const microgltf::AnimationSampler& sampler) :
    m_interpolation(sampler.interpolation)
{
    initInput(model, sampler.input);
    initOutput(model, sampler.output);
}

AnimationSampler::AnimationSampler(AnimationSampler&& other) noexcept :
    m_interpolation(std::exchange(other.m_interpolation, {})),
    m_inputMax(std::exchange(other.m_inputMax, {})),
    m_inputBuffer(std::exchange(other.m_inputBuffer, {})),
    m_outputAttributeSize(std::exchange(other.m_outputAttributeSize, {})),
    m_outputAttributeByteStride(std::exchange(other.m_outputAttributeByteStride, {})),
    m_prevValuePtr(std::exchange(other.m_prevValuePtr, nullptr)),
    m_nextValuePtr(std::exchange(other.m_nextValuePtr, nullptr)),
    m_interpolationTime(std::exchange(other.m_interpolationTime, {})),
    m_currentFrame(std::exchange(other.m_currentFrame, std::numeric_limits<uint64_t>::max()))
{
}

auto AnimationSampler::operator=(AnimationSampler&& other) noexcept -> AnimationSampler&
{
    std::swap(m_interpolation, other.m_interpolation);
    std::swap(m_inputMax, other.m_inputMax);
    std::swap(m_inputBuffer, other.m_inputBuffer);
    std::swap(m_outputAttributeSize, other.m_outputAttributeSize);
    std::swap(m_outputAttributeByteStride, other.m_outputAttributeByteStride);
    std::swap(m_prevValuePtr, other.m_prevValuePtr);
    std::swap(m_nextValuePtr, other.m_nextValuePtr);
    std::swap(m_interpolationTime, other.m_interpolationTime);
    std::swap(m_currentFrame, other.m_currentFrame);
    return *this;
}

auto AnimationSampler::update(const float time) -> void
{
    const auto result = getInput(time);
    m_prevValuePtr = (m_outputBuffer.data() + result.prevIndex * m_outputAttributeByteStride);
    m_nextValuePtr = (m_outputBuffer.data() + result.nextIndex * m_outputAttributeByteStride);
    m_interpolationTime = result.t;
}
