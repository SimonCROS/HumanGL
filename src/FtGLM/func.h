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
    auto radians(vec3 const &vec) -> vec3;
    auto focalLength(float fovRadians) -> float;

    // vec3 related functions
    auto cross(const vec3& v1, const vec3& v2) -> vec3;
    auto scalar(const vec3& v1, const vec3& v2) -> float;
    auto midPoint(const vec3& v1, const vec3& v2) -> vec3;
    auto normalize(const vec3& v) -> vec3;

    // mix - linear interpolation functions
    auto mix(float x, float y, float a) -> float;
    auto mix(vec3 const& x, vec3 const& y, float a) -> vec3;
    auto mix(vec4 const& x, vec4 const& y, float a) -> vec4;

    // mat3 related functions
    auto mat3_cast(quat const& q) -> mat3;

    // mat4 related functions
    auto mat4_cast(quat const& q) -> mat4;
    auto value_ptr(mat4 const& m) -> const float*;
    auto translate(const mat4& m, const vec3& v) -> mat4;
    auto perspective(float fovRadians, float ratio, float near, float far) -> mat4;
    auto rotate(mat4 const& m, float const& angle, vec3 const& v) -> mat4;
    auto scale(mat4 const& m, vec3 const& v) -> mat4;
    auto lookAt(const vec3& position, const vec3& target, const vec3& up) -> mat4;

    // quaternions related functions
    auto dot(const quat& q1, const quat& q2) -> float;
    auto normalize(const quat& q) -> quat;
    auto slerp(const quat& q1, const quat& q2, float t) -> quat;
}

#endif //FUNC_H
