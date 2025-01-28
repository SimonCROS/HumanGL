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


}
