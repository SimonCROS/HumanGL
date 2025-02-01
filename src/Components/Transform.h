//
// Created by Simon Cros on 1/17/25.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

struct Transform
{
    glm::vec3 translation{};
    glm::quat rotation = glm::identity<glm::quat>();
    glm::vec3 scale{1.0f};
};

#endif //TRANSFORM_H
