//
// Created by Simon Cros on 1/29/25.
//

#ifndef OBJECT_H
#define OBJECT_H
#include <memory>
#include <unordered_set>

#include "EngineComponent.h"
#include "Mesh.h"
#include "Components/MeshRenderer.h"

class Engine;

class Object
{
    friend class Engine;

private:
    std::unordered_set<std::unique_ptr<EngineComponent>> m_components;

    auto update(Engine& engine) const -> void
    {
        for (auto& component : m_components)
            component->onUpdate(engine);

        for (auto& component : m_components)
            component->onRender(engine);

        for (auto& component : m_components)
            component->onPostRender(engine);
    }

public:
    template <class T, class... Args>
        requires std::derived_from<T, EngineComponent> && std::constructible_from<T, Args...>
    auto addComponent(Args&&... args) -> T&
    {
        return dynamic_cast<MeshRenderer&>(**m_components.emplace(std::make_unique<T>(std::forward<Args>(args)...)).
                                                          first);
    }
};

#endif //OBJECT_H
