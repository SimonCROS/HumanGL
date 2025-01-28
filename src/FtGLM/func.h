//
// Created by loumarti on 1/28/25.
//

#ifndef FUNC_H
#define FUNC_H

#include "ft_glm.h"
#include <cmath>

namespace ft_glm
{
    // generics
    auto radians(float degree) -> float;
    auto focalLength(float fovRadians) -> float;

    // vec3 related functions
    auto cross(const vec3 &v1, const vec3 &v2) -> vec3;
    auto scalar(const vec3 &v1, const vec3 &v2) -> float;
    auto midPoint(const vec3 &v1, const vec3 &v2) -> vec3;
    auto normalize(const vec3 &v) -> vec3;

    // mat4 related functions
    auto translate(const mat4 &m, const vec3 &v) -> mat4;
    auto perspective(float fovRadians, float ratio, float near, float far) -> mat4;
    auto rotate(mat4 const &m, float const &angle, vec3 const &v) -> mat4;
    auto lookAt(const vec3 &position, const vec3 &target, const vec3 &up) -> mat4;

}

#endif //FUNC_H
