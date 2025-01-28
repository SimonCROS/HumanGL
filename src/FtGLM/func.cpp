//
// Created by loumarti on 1/28/25.
//

#include "FtGLM/func.h"

#include <cmath>


namespace ft_glm
{
    auto radians(float const degree) -> float {
        return static_cast<float>(degree * M_PI) / 180.0f;
    }

    auto focalLength(float const fovRadians) -> float {
        return 1.0f / tanf(fovRadians * 0.5f);
    }

    auto scalar(const vec3 &v1, const vec3 &v2) -> float {
        return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
    }

    auto cross(const vec3 &v1, const vec3 &v2) -> vec3 {
        vec3 crossProduct;

        crossProduct.x = (v1.y * v2.z) - (v1.z * v2.y);
        crossProduct.y = (v1.z * v2.x) - (v1.x * v2.z);
        crossProduct.z = (v1.x * v2.y) - (v1.y * v2.x);

        return crossProduct;
    }

    auto midPoint(const vec3 &v1, const vec3 &v2) -> vec3 {
        return {(v1.x + v2.x) * 0.5f, (v1.y + v2.y) * 0.5f, (v1.z + v2.z) * 0.5f};
    }

    auto normalize(const vec3 &v) -> vec3 {
        const auto invLength = 1 / sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
        return {v.x * invLength, v.y * invLength, v.z * invLength};
    }

    /* -------------------------------- */

    auto translate(const mat4 &m, const vec3 &v) -> mat4
    {
        mat4 translationMatrice = m;

        translationMatrice[3][0] += v.x;
        translationMatrice[3][1] += v.y;
        translationMatrice[3][2] += v.z;

        return translationMatrice;
    }

    auto rotate(mat4 const &m, float const &angle, vec3 const &v) -> mat4 {
        float const a = angle;
        float const c = cos(a);
        float const s = sin(a);

        vec3 axis(normalize(v));
        vec3 temp((1.0f - c) * axis);

        mat4 rotate;
        rotate[0][0] = c + temp[0] * axis[0];
        rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
        rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

        rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
        rotate[1][1] = c + temp[1] * axis[1];
        rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

        rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
        rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
        rotate[2][2] = c + temp[2] * axis[2];

        mat4 rotationMatrix;

        for (int i = 0; i < 4; i++) {
            rotationMatrix[i][0] = m[i][0] * rotate[0][0] + m[i][1] * rotate[0][1] + m[i][2] * rotate[0][2];
            rotationMatrix[i][1] = m[i][0] * rotate[1][0] + m[i][1] * rotate[1][1] + m[i][2] * rotate[1][2];
            rotationMatrix[i][2] = m[i][0] * rotate[2][0] + m[i][1] * rotate[2][1] + m[i][2] * rotate[2][2];
            rotationMatrix[i][3] = m[i][3];
        }

        return rotationMatrix;
    }

    auto perspective(float const fovRadians, float const ratio, float const near, float const far) -> mat4 {
        if (far == near || ratio == 0.0f) {
            std::cerr << "Error inputs perspective" << std::endl;
            return {};
        }
        const auto tanHalfFov = std::tan( fovRadians / 2.0f );
        auto projectionMatrix = mat4(0.0f);

        projectionMatrix[0][0] = 1.0f / (ratio * tanHalfFov);
        projectionMatrix[1][1] = 1.0f / (tanHalfFov);
        projectionMatrix[2][2] = - (far + near) / (far - near);
        projectionMatrix[2][3] = - 1.0f;
        projectionMatrix[3][2] = - (2.0f * far * near) / (far - near);

        return projectionMatrix;
    }

    auto lookAt(const vec3 &position, const vec3 &target, const vec3 &up) -> mat4 {
        const vec3 forward = {normalize(target - position)};
        vec3 right = { normalize(cross(forward, up))};
        const vec3 newUp(cross(right, forward));

        mat4 matrix(1);
        matrix[0][0] = right.x;
        matrix[1][0] = right.y;
        matrix[2][0] = right.z;
        matrix[0][1] = newUp.x;
        matrix[1][1] = newUp.y;
        matrix[2][1] = newUp.z;
        matrix[0][2] = -forward.x;
        matrix[1][2] = -forward.y;
        matrix[2][2] = -forward.z;
        matrix[3][0] = -scalar(right, position);
        matrix[3][1] = -scalar(newUp, position);
        matrix[3][2] = scalar(forward, position);

        return matrix;
    }

    auto dot(const quat& q1, const quat& q2) -> float
    {
        return {q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z};
    }

    auto slerp(const quat& q1, const quat& q2, const float t) -> quat
    {
        quat z = q2;

        float cosTheta = dot(q1, q2);

        // If cosTheta < 0, the interpolation will take the long way around the sphere.
        // To fix this, one quat must be negated.
        if(cosTheta < static_cast<float>(0))
        {
            z = -q2;
            cosTheta = -cosTheta;
        }

        // Perform a linear interpolation when cosTheta is close to 1 to avoid side effect of sin(angle) becoming a zero denominator
        if(cosTheta > static_cast<float>(1) - epsilon<T>())
        {
            // Linear interpolation
            return qua<T, Q>::wxyz(
                mix(x.w, z.w, a),
                mix(x.x, z.x, a),
                mix(x.y, z.y, a),
                mix(x.z, z.z, a));
        }
        else
        {
            // Essential Mathematics, page 467
            T angle = acos(cosTheta);
            return (sin((static_cast<T>(1) - a) * angle) * x + sin(a * angle) * z) / sin(angle);
        }
    }
/*
    auto slerp(const quat& q1, const quat& q2, const float t) -> quat
    {
        float dot = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

        quat q2_corrected = q2;
        if (dot < 0.0f) {
            dot = -dot;
            q2_corrected.w = -q2.w;
            q2_corrected.x = -q2.x;
            q2_corrected.y = -q2.y;
            q2_corrected.z = -q2.z;
        }

        if (dot > 0.9995f) {
            const quat result = {
                q1.w + t * (q2_corrected.w - q1.w),
                q1.x + t * (q2_corrected.x - q1.x),
                q1.y + t * (q2_corrected.y - q1.y),
                q1.z + t * (q2_corrected.z - q1.z)
            };

            const float length = std::sqrt(result.w * result.w + result.x * result.x +
                                     result.y * result.y + result.z * result.z);
            return {result.w / length, result.x / length, result.y / length, result.z / length};
        }

        const float theta_0 = std::acos(dot);
        const float theta = theta_0 * t;
        const float sin_theta = std::sin(theta);
        const float sin_theta_0 = std::sin(theta_0);
        const float s1 = std::cos(theta) - dot * sin_theta / sin_theta_0;
        const float s2 = sin_theta / sin_theta_0;

        return {
            s1 * q1.w + s2 * q2_corrected.w,
            s1 * q1.x + s2 * q2_corrected.x,
            s1 * q1.y + s2 * q2_corrected.y,
            s1 * q1.z + s2 * q2_corrected.z
        };
    }
    */

}
