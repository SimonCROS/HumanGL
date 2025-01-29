//
// Created by Simon Cros on 22/01/2025.
//

#include "AnimationSampler.h"

#include <algorithm>

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
    return {static_cast<size_t>(prevIndex), static_cast<size_t>(nextIndex), stepRatio};
}

AnimationSampler::AnimationSampler(const microgltf::AnimationSampler& sampler) :
    m_interpolation(sampler.interpolation)
{
}

auto AnimationSampler::update(const float time) -> void
{
    const auto result = getInput(time);
    m_prevValuePtr = (m_outputBuffer.data() + result.prevIndex * m_outputAttributeByteStride);
    m_nextValuePtr = (m_outputBuffer.data() + result.nextIndex * m_outputAttributeByteStride);
    m_interpolationTime = result.t;
}
