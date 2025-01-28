//
// Created by loumarti on 1/28/25.
//

#ifndef VEC3_H
#define VEC3_H

#include <iostream>

namespace ft_glm
{
    struct vec3
    {
        float x, y, z;

        vec3() : x(0), y(0), z(0) {}
        vec3(float const x, float const y, float const z) : x(x), y(y), z(z) {}

        auto operator+=(const vec3 &v) -> vec3 & {
            x += v.x;
            y += v.y;
            z += v.z;
            return *this;
        }

        auto operator-=(const vec3 &v) -> vec3 & {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return *this;
        }

        auto operator-() const -> vec3 {
            return {-x, -y, -z};
        }

        auto operator+(const vec3& other) const -> vec3 {
            return {x + other.x, y + other.y, z + other.z};
        }

        auto operator-(const vec3& other) const -> vec3 {
            return {x - other.x, y - other.y, z - other.z};
        }

        auto operator*(float const scalar) const -> vec3 {
            return {x * scalar, y * scalar, z * scalar};
        }

        auto operator/(float const scalar) const -> vec3 {
            return {x / scalar, y / scalar, z / scalar};
        }

        auto operator*(const vec3& other) const -> vec3 {
            return {x * other.x, y * other.y, z * other.z};
        }

        auto operator/(const vec3& other) const -> vec3 {
            return {x / other.x, y / other.y, z / other.z};
        }
    };
};


#endif //VEC3_H

