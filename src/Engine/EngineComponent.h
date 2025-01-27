//
// Created by Simon Cros on 1/27/25.
//

#ifndef ENGINECOMPONENT_H
#define ENGINECOMPONENT_H

class Engine;

class EngineComponent {
public:
    virtual ~EngineComponent() = default;

    virtual auto onUpdate(Engine& engine) -> void {}
    virtual auto onPostRender(Engine& engine) -> void {}
};

#endif //ENGINECOMPONENT_H
