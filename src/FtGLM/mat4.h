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

        mat4()
            : columns{
                {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f},
                {0.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 0.0f}
            }
        {
        }

        mat4(float x0, float y0, float z0, float w0,
             float x1, float y1, float z1, float w1,
             float x2, float y2, float z2, float w2,
             float x3, float y3, float z3, float w3)
            : columns{
                    {x0, y0, z0, w0},
                    {x1, y1, z1, w1},
                    {x2, y2, z2, w2},
                    {x3, y3, z3, w3}
            }
        {
        }

        mat4(const vec4& c1, const vec4& c2, const vec4& c3, const vec4& c4)
            : columns{c1, c2, c3, c4}
        {
        }

        explicit mat4(float diagonal)
            : columns{
                {diagonal, 0.0f, 0.0f, 0.0f}, {0.0f, diagonal, 0.0f, 0.0f},
                {0.0f, 0.0f, diagonal, 0.0f}, {0.0f, 0.0f, 0.0f, diagonal}
            }
        {
        }

        mat4(const mat4& m)
            : columns{m[0], m[1], m[2], m[3]}
        {
        }


        explicit mat4(const mat3& m)
            : columns{
                {m[0][0], m[0][1], m[0][2], 0.0f},
                {m[1][0], m[1][1], m[1][2], 0.0f},
                {m[2][0], m[2][1], m[2][2], 0.0f},
                {0.0f, 0.0f, 0.0f, 1.0f}
            }
        {
        }

        auto operator-() const -> mat4
        {
            return {
                -columns[0],
                -columns[1],
                -columns[2],
                -columns[3]
            };
        }

        auto operator+(const mat4& other) const -> mat4
        {
            return {
                columns[0] + other.columns[0],
                columns[1] + other.columns[1],
                columns[2] + other.columns[2],
                columns[3] + other.columns[3]
            };
        }

        auto operator-(const mat4& other) const -> mat4
        {
            return {
                columns[0] - other.columns[0],
                columns[1] - other.columns[1],
                columns[2] - other.columns[2],
                columns[3] - other.columns[3]
            };
        }

        auto operator*(float const scalar) const -> mat4
        {
            return {
                columns[0] * scalar,
                columns[1] * scalar,
                columns[2] * scalar,
                columns[3] * scalar
            };
        }

        auto operator*(const mat4& other) const -> mat4
        {
            auto result = mat4(0.0f);
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    result.columns[j].x += columns[i].x * other.columns[j][i];
                    result.columns[j].y += columns[i].y * other.columns[j][i];
                    result.columns[j].z += columns[i].z * other.columns[j][i];
                    result.columns[j].w += columns[i].w * other.columns[j][i];
                }
            }
            return result;
        }

        auto operator*(const vec4& vec) const -> vec4
        {
            vec4 result;
            result.x = columns[0].x * vec.x + columns[1].x * vec.y + columns[2].x * vec.z + columns[3].x * vec.w;
            result.y = columns[0].y * vec.x + columns[1].y * vec.y + columns[2].y * vec.z + columns[3].y * vec.w;
            result.z = columns[0].z * vec.x + columns[1].z * vec.y + columns[2].z * vec.z + columns[3].z * vec.w;
            result.w = columns[0].w * vec.x + columns[1].w * vec.y + columns[2].w * vec.z + columns[3].w * vec.w;
            return result;
        }

        auto operator*=(const mat4& other) -> mat4&
        {
            mat4 result;
            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    result.columns[i][j] =
                        columns[0][j] * other.columns[i][0] +
                        columns[1][j] * other.columns[i][1] +
                        columns[2][j] * other.columns[i][2] +
                        columns[3][j] * other.columns[i][3];
                }
            }
            *this = result;
            return *this;
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

        auto operator==(const mat4& other) const -> bool
        {
            for (int i = 0; i < 4; ++i)
            {
                if (columns[i] != other.columns[i])
                {
                    return false;
                }
            }
            return true;
        }

        auto operator!=(const mat4& other) const -> bool
        {
            return !(*this == other);
        }

        static auto identity() -> mat4
        {
            return mat4(1.0f);
        }
    };

    inline auto operator<<(std::ostream& os, const mat4& m) -> std::ostream&
    {
        os << "[" << std::endl;
        for (int i = 0; i < 4; ++i)
        {
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
