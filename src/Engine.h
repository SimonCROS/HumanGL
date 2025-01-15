//
// Created by Simon Cros on 1/13/25.
//

#ifndef ENGINE_H
#define ENGINE_H
#include "Window.h"

class Engine
{
private:
    Window m_window;

public:
    explicit Engine(Window&& window) noexcept : m_window(std::move(window))
    {
    }

    [[nodiscard]] auto getWindow() noexcept -> Window&
    {
        return m_window;
    }

    [[nodiscard]] auto getWindow() const noexcept -> const Window&
    {
        return m_window;
    }
};

#endif //ENGINE_H
