//
// Created by loumarti on 1/28/25.
//

#ifndef VEC2_H
#define VEC2_H

#include <iostream>

namespace ft_glm {

    struct vec2 {
        float x, y;
        vec2() : x(0), y(0) {}
        vec2(const float x, const float y) : x(x), y(y) {}

        auto operator+(const vec2& other) const -> vec2 {
            return {x + other.x, y + other.y};
        }

        auto operator-(const vec2& other) const -> vec2 {
            return {x - other.x, y - other.y};
        }

        auto operator*(float const scalar) const -> vec2 {
            return {x * scalar, y * scalar};
        }

        auto operator*(const vec2& other) const -> vec2 {
            return {x * other.x, y * other.y};
        }

        auto operator/(float const scalar) const -> vec2 {
            return {x / scalar, y / scalar};
        }

        auto operator/(const vec2& other) const -> vec2 {
            return {x / other.x, y / other.y};
        }

    };

    inline auto operator<<(std::ostream& os, const vec2& v) -> std::ostream&  {
            os << "(" << v.x << ", " << v.y << ")";
        return os;
    }
}



#endif //VEC2_H
