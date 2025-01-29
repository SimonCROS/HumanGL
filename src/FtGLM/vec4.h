//
// Created by loumarti on 1/28/25.
//

#ifndef VEC4_H
#define VEC4_H

#include <assert.h>

namespace ft_glm
{
    struct vec4
    {
        float x, y, z, w;

        vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
        {
        }

        vec4(float const x, float const y, float const z) : x(x), y(y), z(z), w(1.0f)
        {
        }

        vec4(float const x, float const y, float const z, float const w) : x(x), y(y), z(z), w(w)
        {
        }

        vec4(const vec4& other)
        {
            x = other.x;
            y = other.y;
            z = other.z;
            w = other.w;
        }

        auto operator=(const vec4& other) -> vec4&
        {
            if (this != &other)
            {
                x = other.x;
                y = other.y;
                z = other.z;
                w = other.w;
            }
            return *this;
        }

        auto operator[](int const index) -> float&
        {
            assert(index >= 0 && index < 4);
            return (&x)[index];
        }

        auto operator[](int const index) const -> const float&
        {
            assert(index >= 0 && index < 4);
            return (&x)[index];
        }

        auto operator+=(const vec4& v) -> vec4&
        {
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
            return *this;
        }

        auto operator-=(const vec4& v) -> vec4&
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
            return *this;
        }

        auto operator-() const -> vec4
        {
            return {-x, -y, -z, -w};
        }

        auto operator+(const vec4& other) const -> vec4
        {
            return {x + other.x, y + other.y, z + other.z, w + other.w};
        }

        auto operator-(const vec4& other) const -> vec4
        {
            return {x - other.x, y - other.y, z - other.z, w - other.w};
        }

        auto operator*(float const scalar) const -> vec4
        {
            return {x * scalar, y * scalar, z * scalar, w * scalar};
        }

        auto operator/(float const scalar) const -> vec4
        {
            return {x / scalar, y / scalar, z / scalar, w / scalar};
        }

        auto operator*(const vec4& other) const -> vec4
        {
            return {x * other.x, y * other.y, z * other.z, w * other.w};
        }

        auto operator/(const vec4& other) const -> vec4
        {
            return {x / other.x, y / other.y, z / other.z, w / other.w};
        }
    };

    inline auto operator==(const vec4& l, const vec4& r) -> bool
    {
        return l.x == r.x && l.y == r.y && l.z == r.z && l.w == r.w;
    }

    inline auto operator!=(const vec4& l, const vec4& r) -> bool
    {
        return l.x != r.x || l.y != r.y || l.z != r.z || l.w != r.w;
    }

    inline auto operator<<(std::ostream& os, const vec4& v) -> std::ostream&
    {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
        return os;
    }
}
#endif //VEC4_H
