#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <string>
#include <unordered_map>

#include "Expected.h"
#include "Shader.h"
#include "StringUnorderedMap.h"
#include "glm/mat4x4.hpp"

#include "glad/gl.h"

#define CUSTOM_MAX_VERTEX_ATTRIBUTES 16

class ShaderProgramInstance
{
    GLuint m_id;
    Shader m_vertShader;
    Shader m_fragShader;

public:
    static auto Create(const std::string_view& vertexCode, const std::string_view& fragCode)
        -> Expected<ShaderProgramInstance, std::string>;

    ShaderProgramInstance() : m_id(0)
    {
    }

    ShaderProgramInstance(GLuint id, Shader&& vertShader, Shader&& fragShader);

    ShaderProgramInstance(const ShaderProgramInstance&) = delete;

    ShaderProgramInstance(ShaderProgramInstance&& other) noexcept
        : m_id(std::exchange(other.m_id, 0)),
          m_vertShader(std::exchange(other.m_vertShader, {})),
          m_fragShader(std::exchange(other.m_fragShader, {}))
    {
    }

    ~ShaderProgramInstance()
    {
        glDeleteProgram(m_id);
    }

    auto operator=(const ShaderProgramInstance&) -> ShaderProgramInstance& = delete;

    auto operator=(ShaderProgramInstance&& other) noexcept -> ShaderProgramInstance&
    {
        std::swap(m_id, other.m_id);
        std::swap(m_vertShader, other.m_vertShader);
        std::swap(m_fragShader, other.m_fragShader);
        return *this;
    }

    [[nodiscard]] auto id() const -> GLuint { return m_id; }

    void use() const;

    [[nodiscard]] bool hasAttribute(const std::string_view& attribute) const;
    [[nodiscard]] GLint getAttributeLocation(const std::string_view& attribute) const;
    void enableAttribute(GLuint attribute);
    void disableAttribute(GLuint attribute);
    void applyAttributeChanges();

    void setBool(const std::string_view& name, bool value);
    void setInt(const std::string_view& name, GLint value);
    void setUint(const std::string_view& name, GLuint value);
    void setFloat(const std::string_view& name, float value);
    void setVec3(const std::string_view& name, const glm::vec3& value);
    void setVec4(const std::string_view& name, const glm::vec4& value);
    void setMat4(const std::string_view& name, const glm::mat4& value);

private:
    StringUnorderedMap<GLint> m_attributeLocations;
    StringUnorderedMap<bool> m_bools;
    StringUnorderedMap<GLint> m_ints;
    StringUnorderedMap<GLuint> m_uints;
    StringUnorderedMap<float> m_floats;
    StringUnorderedMap<glm::vec3> m_vec3s;
    StringUnorderedMap<glm::vec4> m_vec4s;
    StringUnorderedMap<glm::mat4> m_mat4s;
    // Real map of enabled attributes
    bool m_enabledAttributes[CUSTOM_MAX_VERTEX_ATTRIBUTES] = {false};
    // Attributes that will be enabled for the next reset
    bool m_currentEnabledAttributes[CUSTOM_MAX_VERTEX_ATTRIBUTES] = {false};

    static auto linkProgram(GLuint id) -> Expected<void, std::string>;

    template <typename T>
    static bool storeUniformValue(const std::string_view& name, T value, StringUnorderedMap<T>& map,
                                  const std::string** nullTerminatedString)
    {
        const auto it = map.find(name);
        if (it == map.end())
        {
            const auto inserted = map.emplace(std::string(name), value);
            *nullTerminatedString = &inserted.first->first;
            return true;
        }

        *nullTerminatedString = &it->first;
        if (it->second != value)
        {
            it->second = value;
            return true;
        }
        return false;
    }
};

#endif
