//
// Created by Simon Cros on 1/13/25.
//

#include "Engine.h"

auto Engine::run(const LoopCallbackType& callback) -> void
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    m_start = ClockType::now();

    auto previousTime = m_start;
    while (m_window.update())
    {
        std::invoke(callback, *this);

        m_window.swapBuffers();

        const auto newTime = ClockType::now();
        ++m_currentFrameInfo.frameCount;
        m_currentFrameInfo.deltaTime = newTime - previousTime;
        previousTime = newTime;
    }
}
