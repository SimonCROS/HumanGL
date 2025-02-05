#include <fstream>
#include <sstream>
#include <iostream>

#include "glad/gl.h"
// #include "logger.h"
#include "ShaderProgram.h"
#include "Shader.h"

GLuint ShaderProgram::currentShaderProgramId = 0;

ShaderProgram::ShaderProgram(const std::string_view& vertexCode,
                             const std::string_view& fragmentCode) : id(0), m_vertId(0), m_fragId(0)
// id 0 is ignored with glDeleteProgram
{
    m_vertId = Shader::createShader(vertexCode, GL_VERTEX_SHADER);
    m_fragId = Shader::createShader(fragmentCode, GL_FRAGMENT_SHADER);

    if (m_vertId == 0 || m_fragId == 0)
    {
        // If one is not 0, delete it
        Shader::destroyShader(m_fragId);
        Shader::destroyShader(m_vertId);
        m_fragId = 0;
        m_vertId = 0;
        return;
    }

    id = glCreateProgram();
    glAttachShader(id, m_vertId);
    glAttachShader(id, m_fragId);

    if (!linkProgram(id))
    {
        destroy();
        return;
    }

    m_attributeLocations["POSITION"] = 0;
    m_attributeLocations["NORMAL"] = 1;
    m_attributeLocations["TEXCOORD_0"] = 2;
}

auto ShaderProgram::destroy() -> void
{
    glDeleteProgram(id);
    Shader::destroyShader(m_fragId);
    Shader::destroyShader(m_vertId);
    id = 0;
    m_fragId = 0;
    m_vertId = 0;
}

auto ShaderProgram::use() const -> void
{
    if (currentShaderProgramId != id)
    {
        glUseProgram(id);
        currentShaderProgramId = id;
    }
}

auto ShaderProgram::hasAttribute(const std::string_view& attribute) const -> bool
{
    return m_attributeLocations.contains(attribute);
}

auto ShaderProgram::getAttributeLocation(const std::string_view& attribute) const -> GLint
{
    const auto it = m_attributeLocations.find(attribute);
    return (it != m_attributeLocations.cend()) ? it->second : -1;
}

auto ShaderProgram::enableAttribute(const GLuint attribute) -> void
{
    assert(attribute < CUSTOM_MAX_VERTEX_ATTRIBUTES);
    m_currentEnabledAttributes[attribute] = true;
}

auto ShaderProgram::disableAttribute(const GLuint attribute) -> void
{
    assert(attribute < CUSTOM_MAX_VERTEX_ATTRIBUTES);
    m_currentEnabledAttributes[attribute] = false;
}

auto ShaderProgram::applyAttributeChanges() -> void
{
    for (size_t i = 0; i < 16; i++)
    {
        if (m_currentEnabledAttributes[i]) // Should enable attribute
        {
            if (!m_enabledAttributes[i]) // Attribute not already enabled
            {
                glEnableVertexAttribArray(i);
                m_enabledAttributes[i] = true;
            }
            m_currentEnabledAttributes[i] = false; // Reset for next call
        }
        else if (m_enabledAttributes[i]) // Attribute was enabled
        {
            glDisableVertexAttribArray(i);
            m_enabledAttributes[i] = false;
        }
    }
}

auto ShaderProgram::setBool(const std::string_view& name, const bool value) -> void
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_bools, &nullTerminated))
    {
        glUniform1i(glGetUniformLocation(id, nullTerminated->c_str()), static_cast<GLint>(value));
    }
}

void ShaderProgram::setInt(const std::string_view& name, const GLint value)
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_ints, &nullTerminated))
    {
        glUniform1i(glGetUniformLocation(id, nullTerminated->c_str()), value);
    }
}

void ShaderProgram::setUint(const std::string_view& name, const GLuint value)
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_uints, &nullTerminated))
    {
        glUniform1ui(glGetUniformLocation(id, nullTerminated->c_str()), value);
    }
}

auto ShaderProgram::setFloat(const std::string_view& name, const float value) -> void
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_floats, &nullTerminated))
    {
        glUniform1f(glGetUniformLocation(id, nullTerminated->c_str()), value);
    }
}

auto ShaderProgram::setVec3(const std::string_view& name, const ft_glm::vec3& value) -> void
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_vec3s, &nullTerminated))
    {
        glUniform3f(glGetUniformLocation(id, nullTerminated->c_str()), value.x, value.y, value.z);
    }
}

auto ShaderProgram::setVec4(const std::string_view& name, const ft_glm::vec4& value) -> void
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_vec4s, &nullTerminated))
    {
        glUniform4f(glGetUniformLocation(id, nullTerminated->c_str()), value.x, value.y, value.z, value.w);
    }
}

auto ShaderProgram::setMat4(const std::string_view& name, const ft_glm::mat4& value) -> void
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_mat4s, &nullTerminated))
    {
        glUniformMatrix4fv(glGetUniformLocation(id, nullTerminated->c_str()), 1, GL_FALSE, ft_glm::value_ptr(value));
    }
}

auto ShaderProgram::linkProgram(const GLuint id) -> bool
{
    int success;

    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        // char infoLog[1024];
        // glGetShaderInfoLog(id, 1024, nullptr, infoLog);
        // logError2(FTRUN_SHADER_PROGRAM, FTRUN_COMPILATION_FAILED, infoLog);
        return false;
    }

    return true;
}
