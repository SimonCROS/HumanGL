//
// Created by loumarti on 1/28/25.
//

#ifndef QUAT_H
#define QUAT_H

#include "ft_glm.h"
#include <cmath>

namespace ft_glm
{
    struct quat
    {
        float w, x, y, z;

        quat() : w(0.0f), x(0.0f), y(0.0f), z(0.0f)
        {
        }

        quat(const float w, const float x, const float y, const float z) : w(w), x(x), y(y), z(z)
        {
        }

        explicit quat(const float angle, const vec3& axis)
        {
            const float halfAngle = angle * 0.5f;
            const float s = std::sin(halfAngle);
            w = std::cos(halfAngle);
            x = axis.x * s;
            y = axis.y * s;
            z = axis.z * s;
        }

        explicit quat(const vec3& v)
        {
            const vec3 eulerAngle(v.x * 0.5f, v.y * 0.5f, v.z * 0.5f);
            const vec3 c(std::cos(eulerAngle.x), std::cos(eulerAngle.y), std::cos(eulerAngle.z));
            const vec3 s(std::sin(eulerAngle.x), std::sin(eulerAngle.y), std::sin(eulerAngle.z));

            this->w = c.x * c.y * c.z + s.x * s.y * s.z;
            this->x = s.x * c.y * c.z - c.x * s.y * s.z;
            this->y = c.x * s.y * c.z + s.x * c.y * s.z;
            this->z = c.x * c.y * s.z - s.x * s.y * c.z;

        }

        explicit quat(const float pitch, const float yaw, const float roll)
        {
            const vec3 eulerAngle(pitch * 0.5f, yaw * 0.5f, roll * 0.5f);
            const vec3 c(std::cos(eulerAngle.x), std::cos(eulerAngle.y), std::cos(eulerAngle.z));
            const vec3 s(std::sin(eulerAngle.x), std::sin(eulerAngle.y), std::sin(eulerAngle.z));

            this->w = c.x * c.y * c.z + s.x * s.y * s.z;
            this->x = s.x * c.y * c.z - c.x * s.y * s.z;
            this->y = c.x * s.y * c.z + s.x * c.y * s.z;
            this->z = c.x * c.y * s.z - s.x * s.y * c.z;
        }

        auto operator-() const -> quat
        {
            return {-w, -x, -y, -z};
        }

        auto operator+(const quat& other) const -> quat
        {
            return {w + other.w, x + other.x, y + other.y, z + other.z};
        }

        auto operator*(const quat& other) const -> quat
        {
            return {
                w * other.w - x * other.x - y * other.y - z * other.z,
                w * other.x + x * other.w + y * other.z - z * other.y,
                w * other.y + y * other.w + z * other.x - x * other.z,
                w * other.z + z * other.w + x * other.y - y * other.x,

            };
        }

        auto operator[](int const index) -> float&
        {
            assert(index >= 0 && index < 4);
            switch (index)
            {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: return x;
            }
        }

        auto operator[](int const index) const -> const float&
        {
            assert(index >= 0 && index < 4);
            switch (index)
            {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
            default: return x;
            }
        }


        static auto identity() -> quat
        {
            return {1.0f, 0.0f, 0.0f, 0.0f};
        }
    };

    inline auto crossVec3(const vec3& v1, const vec3& v2) -> vec3
    {
        vec3 crossProduct;

        crossProduct.x = (v1.y * v2.z) - (v1.z * v2.y);
        crossProduct.y = (v1.z * v2.x) - (v1.x * v2.z);
        crossProduct.z = (v1.x * v2.y) - (v1.y * v2.x);

        return crossProduct;
    }

    inline auto operator*(const quat& q, const vec3& v) -> vec3
    {
        vec3 const QuatVector(q.x, q.y, q.z);
        vec3 const uv(crossVec3(QuatVector, v));
        vec3 const uuv(crossVec3(QuatVector, uv));

        return v + ((uv * q.w) + uuv) * 2.0f;
    }

    inline auto operator*(const quat& q, const float scalar) -> quat
    {
        return {q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar};
    }

    inline auto operator*(const float scalar, const quat& q) -> quat
    {
        return {q.w * scalar, q.x * scalar, q.y * scalar, q.z * scalar};
    }

    inline auto operator<<(std::ostream& os, const quat& q) -> std::ostream&
    {
        os << "quat(" << q.w << ", {" << q.y << ", " << q.z << ", " << q.w << "})";
        return os;
    }

    inline auto operator/(const quat& q, const float scalar) -> quat
    {
        return {q.w / scalar, q.x / scalar, q.y / scalar, q.z / scalar};
    }

    inline auto operator/(const float scalar, const quat& q) -> quat
    {
        return {scalar / q.w, scalar / q.x, scalar / q.y, scalar / q.z};
    }
}


#endif //QUAT_H
