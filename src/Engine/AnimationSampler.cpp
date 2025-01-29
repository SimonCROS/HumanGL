//
// Created by Simon Cros on 22/01/2025.
//

#include "AnimationSampler.h"

#include <algorithm>

auto AnimationSampler::getInput(const float time) const -> InputResult
{
    const auto begin = m_inputBuffer.data;
    const auto end = begin + m_inputBuffer.size;
    const auto upperBound = std::upper_bound(begin, end, time);

    // Clamp to first and last
    if (upperBound == begin)
        return {0, 0, 0};
    if (upperBound == end)
        return {m_inputBuffer.size - 1, m_inputBuffer.size - 1, 1};

    const auto next = upperBound;
    const auto prev = upperBound - 1;

    const auto prevVal = *prev;
    const auto nextVal = *next;
    const auto prevIndex = prev - m_inputBuffer.data;
    const auto nextIndex = next - m_inputBuffer.data;

    const float stepRatio = (nextVal != prevVal) ? ((time - prevVal) / (nextVal - prevVal)) : 0;
    return {static_cast<size_t>(prevIndex), static_cast<size_t>(nextIndex), stepRatio};
}

AnimationSampler::AnimationSampler(const microgltf::AnimationSamplerInterpolation interpolation,
                                   const Animation::InputBuffer& inputBuffer,
                                   const Animation::OutputBuffer& outputBuffer)
    : m_interpolation(interpolation),
      m_inputBuffer(inputBuffer),
      m_outputBuffer(outputBuffer)
{
}

auto AnimationSampler::update(const float time) -> void
{
    const auto result = getInput(time);
    m_prevValuePtr = (m_outputBuffer.data + result.prevIndex * m_outputBuffer.attributeByteStride);
    m_nextValuePtr = (m_outputBuffer.data + result.nextIndex * m_outputBuffer.attributeByteStride);
    m_interpolationTime = result.t;
}
