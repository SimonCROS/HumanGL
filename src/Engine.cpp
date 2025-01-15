//
// Created by Simon Cros on 1/13/25.
//

#include "Engine.h"

auto Engine::run(const LoopCallbackType& callback) -> void
{
    m_window.setAsCurrentContext();

    while (m_window.update())
    {
        std::invoke(callback, *this);

        m_window.swapBuffers();
    }
}
