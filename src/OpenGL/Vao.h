//
// Created by Simon Cros on 2/6/25.
//

#ifndef VAO_H
#define VAO_H
#include <unordered_map>
#include <glad/gl.h>

#include "StringUnorderedMap.h"
#include "Utility/EnumHelpers.h"

enum VaoFlags : unsigned char
{
    VaoHasNone = 0,
    VaoHasPosition = 1 << 0,
    VaoHasNormal = 1 << 0,
    VaoHasColor0 = 1 << 1,
    VaoHasTexCoord0 = 1 << 2,
};

MAKE_FLAG_ENUM(VaoFlags)

class Vao
{
private:
    VaoFlags m_flags{VaoHasNone};
    GLuint m_id{0};
    GLuint m_currentlyBoundArrayBuffer{0};
    GLuint m_currentlyBoundArrayElementBuffer{0};

    static inline const StringUnorderedMap<int> attributeLocations{
        {"POSITION", 0},
        {"NORMAL", 1},
        {"COLOR_0", 2},
        {"TEXCOORD_0", 3},
    };

public:
    static auto Create(VaoFlags flags) -> Vao;

    Vao() = default;

    Vao(const VaoFlags flags, const GLuint id) : m_flags(flags), m_id(id)
    {
    }

    Vao(const Vao&) = delete;

    Vao(Vao&& other) noexcept : m_flags(other.m_flags), m_id(std::exchange(other.m_id, 0))
    {
    }

    ~Vao()
    {
        glDeleteVertexArrays(1, &m_id);
    }

    auto operator=(const Vao&) -> Vao& = delete;

    auto operator=(Vao&& other) noexcept -> Vao&
    {
        m_flags = other.m_flags;
        std::swap(m_id, other.m_id);
        return *this;
    }

    [[nodiscard]] auto id() const -> GLuint { return m_id; }

    [[nodiscard]] static auto hasAttribute(const std::string_view& attribute) -> bool
    {
        return attributeLocations.contains(attribute);
    }

    [[nodiscard]] static auto getAttributeLocation(const std::string_view& attribute) -> GLint
    {
        const auto it = attributeLocations.find(attribute);
        return (it != attributeLocations.cend()) ? it->second : -1;
    }
};


#endif //VAO_H
