//
// Created by loumarti on 1/28/25.
//

#ifndef MAT4_H
#define MAT4_H

namespace ft_glm
{
    struct mat4
    {
        vec4 columns[4];

        /*
        m00  m01  m02  m03                  00  01  02  03
        m10  m11  m12  m13                  04  05  06  07
        m20  m21  m22  m23                  08  09  10  11
        m30  m31  m32  m33                  12  13  14  15
        */

        mat4()
            : columns{{0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f},
                      {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}} {}

        mat4(const vec4& c1, const vec4& c2, const vec4& c3, const vec4& c4)
            : columns{c1, c2, c3, c4} {}

        explicit mat4(float diagonal)
            : columns{{diagonal, 0.0f, 0.0f, 0.0f}, {0.0f, diagonal, 0.0f, 0.0f},
                      {0.0f, 0.0f, diagonal, 0.0f}, {0.0f, 0.0f, 0.0f, diagonal}} {}

        auto operator+(const mat4& other) const -> mat4 {
            return {
                columns[0] + other.columns[0],
                columns[1] + other.columns[1],
                columns[2] + other.columns[2],
                columns[3] + other.columns[3]
            };
        }

        auto operator-(const mat4& other) const -> mat4 {
            return {
                columns[0] - other.columns[0],
                columns[1] - other.columns[1],
                columns[2] - other.columns[2],
                columns[3] - other.columns[3]
            };
        }

        auto operator*(float const scalar) const -> mat4 {
            return {
                columns[0] * scalar,
                columns[1] * scalar,
                columns[2] * scalar,
                columns[3] * scalar
            };
        }

        auto operator*(const mat4& other) const -> mat4 {
            auto result = mat4(0.0f);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.columns[j].x += columns[i].x * other.columns[j][i];
                    result.columns[j].y += columns[i].y * other.columns[j][i];
                    result.columns[j].z += columns[i].z * other.columns[j][i];
                    result.columns[j].w += columns[i].w * other.columns[j][i];
                }
            }
            return result;
        }

        auto operator*(const vec4& vec) const -> vec4 {
            vec4 result;
            result.x = columns[0].x * vec.x + columns[1].x * vec.y + columns[2].x * vec.z + columns[3].x * vec.w;
            result.y = columns[0].y * vec.x + columns[1].y * vec.y + columns[2].y * vec.z + columns[3].y * vec.w;
            result.z = columns[0].z * vec.x + columns[1].z * vec.y + columns[2].z * vec.z + columns[3].z * vec.w;
            result.w = columns[0].w * vec.x + columns[1].w * vec.y + columns[2].w * vec.z + columns[3].w * vec.w;
            return result;
        }


        auto operator[](int const index) -> vec4&
        {
            assert(index >= 0 && index < 4);
            return columns[index];
        }

        auto operator[](int const index) const -> const vec4&
        {
            assert(index >= 0 && index < 4);
            return columns[index];
        }

        auto operator==(const mat4& other) const -> bool {
            for (int i = 0; i < 4; ++i) {
                if (columns[i] != other.columns[i]) {
                    return false;
                }
            }
            return true;
        }

        auto operator!=(const mat4& other) const -> bool {
            return !(*this == other);
        }
    };

    inline auto operator<<(std::ostream& os, const mat4& m) -> std::ostream& {
        os << "[" << std::endl;
        for (int i = 0; i < 4; ++i) {
            os << "  ("
               << m.columns[0][i] << ", "
               << m.columns[1][i] << ", "
               << m.columns[2][i] << ", "
               << m.columns[3][i]
               << ")" << std::endl;
        }
        os << "]" << std::endl;
        return os;
    }

}

#endif //MAT4_H
