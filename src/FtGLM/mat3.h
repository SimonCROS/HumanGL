//
// Created by loumarti on 1/29/25.
//

#ifndef MAT3_H
#define MAT3_H

#include "vec3.h"

namespace ft_glm
{
    struct mat3
    {
        vec3 columns[3];

        /*
        m00  m01  m02                   00  01  02
        m10  m11  m12                   03  04  05
        m20  m21  m22                   06  07  08
        */

        mat3()
            : columns{
                {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f},
                {0.0f, 0.0f, 0.0f}
            }
        {
        }

        mat3(const vec3& c1, const vec3& c2, const vec3& c3)
            : columns{c1, c2, c3}
        {
        }

        explicit mat3(float diagonal)
            : columns{
                {diagonal, 0.0f, 0.0f}, {0.0f, diagonal, 0.0f},
                {0.0f, 0.0f, diagonal}
            }
        {
        }

        auto operator+(const mat3& other) const -> mat3
        {
            return {
                columns[0] + other.columns[0],
                columns[1] + other.columns[1],
                columns[2] + other.columns[2]
            };
        }

        auto operator-(const mat3& other) const -> mat3
        {
            return {
                columns[0] - other.columns[0],
                columns[1] - other.columns[1],
                columns[2] - other.columns[2],
            };
        }

        auto operator*(float const scalar) const -> mat3
        {
            return {
                columns[0] * scalar,
                columns[1] * scalar,
                columns[2] * scalar,
            };
        }

        auto operator*(const mat3& other) const -> mat3
        {
            auto result = mat3(0.0f);
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    result.columns[j].x += columns[i].x * other.columns[j][i];
                    result.columns[j].y += columns[i].y * other.columns[j][i];
                    result.columns[j].z += columns[i].z * other.columns[j][i];
                }
            }
            return result;
        }

        auto operator*(const vec3& vec) const -> vec3
        {
            vec3 result;
            result.x = columns[0].x * vec.x + columns[1].x * vec.y + columns[2].x * vec.z;
            result.y = columns[0].y * vec.x + columns[1].y * vec.y + columns[2].y * vec.z;
            result.z = columns[0].z * vec.x + columns[1].z * vec.y + columns[2].z * vec.z;
            return result;
        }


        auto operator[](int const index) -> vec3&
        {
            assert(index >= 0 && index < 3);
            return columns[index];
        }

        auto operator[](int const index) const -> const vec3&
        {
            assert(index >= 0 && index < 3);
            return columns[index];
        }

        auto operator==(const mat3& other) const -> bool
        {
            for (int i = 0; i < 3; ++i)
            {
                if (columns[i] != other.columns[i])
                {
                    return false;
                }
            }
            return true;
        }

        auto operator!=(const mat3& other) const -> bool
        {
            return !(*this == other);
        }
    };

    inline auto operator<<(std::ostream& os, const mat3& m) -> std::ostream&
    {
        os << "[" << std::endl;
        for (int i = 0; i < 3; ++i)
        {
            os << "  ("
                << m.columns[0][i] << ", "
                << m.columns[1][i] << ", "
                << m.columns[2][i] << ", "
                << ")" << std::endl;
        }
        os << "]" << std::endl;
        return os;
    }
}

#endif //MAT3_H
