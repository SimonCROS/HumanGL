//
// Created by loumarti on 1/28/25.
//

#ifndef VEC3_H
#define VEC3_H

#include <iostream>
#include <assert.h>

namespace ft_glm
{
    struct vec3
    {
        float x, y, z;

        vec3() : x(0), y(0), z(0) {}
        explicit vec3(const float v) : x(v), y(v), z(v) {}
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

        auto operator[](int const index) -> float&
        {
            assert(index >= 0 && index < 3);
            return (&x)[index];
        }

        auto operator[](int const index) const -> const float &
        {
            assert(index >= 0 && index < 3);
            return (&x)[index];
        }

    };

    inline auto operator*(const float s, const vec3 &v) -> vec3 {
        return {s * v.x, s * v.y, s * v.z};
    }

    auto inline  operator<<(std::ostream& os, const vec3& v) -> std::ostream&  {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
}


#endif //VEC3_H

