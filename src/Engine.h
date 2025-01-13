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
};

#endif //ENGINE_H
