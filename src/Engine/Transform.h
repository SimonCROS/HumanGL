//
// Created by Simon Cros on 1/17/25.
//

#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "../FtGLM/ft_glm.h"

struct Transform
{
    ft_glm::vec3 translation{};
    ft_glm::quat rotation = ft_glm::quat::identity();
    ft_glm::vec3 scale{1.0f};

    [[nodiscard]] auto trs() const -> ft_glm::mat4
    {
        auto mat = ft_glm::mat4::identity();
        mat = ft_glm::translate(mat, translation);
        mat *= ft_glm::mat4_cast(rotation);
        mat = ft_glm::scale(mat, scale);
        return mat;
    }
};

#endif //TRANSFORM_H
