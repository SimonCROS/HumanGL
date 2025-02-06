#include <fstream>
#include <sstream>
#include <iostream>

#include "glad/gl.h"
#include "ShaderProgramInstance.h"

#include "Shader.h"
#include "FtGLM/ft_glm.h"

auto ShaderProgramInstance::Create(const std::string_view& vertexCode, const std::string_view& fragCode)
    -> Expected<ShaderProgramInstance, std::string>
{
    auto e_vertShader = Shader::Create(GL_VERTEX_SHADER, vertexCode);
    if (!e_vertShader)
        return Unexpected(std::move(e_vertShader).error());

    auto e_fragShader = Shader::Create(GL_FRAGMENT_SHADER, fragCode);
    if (!e_fragShader)
        return Unexpected(std::move(e_fragShader).error());

    const auto id = glCreateProgram();
    if (id == 0)
        return Unexpected("Failed to create new program id");

    glAttachShader(id, e_vertShader->id());
    glAttachShader(id, e_fragShader->id());

    if (auto e_linkResult = linkProgram(id); !e_linkResult)
    {
        glDeleteProgram(id);
        return Unexpected(std::move(e_linkResult).error());
    }

    return Expected<ShaderProgramInstance, std::string>{
        std::in_place, id, *std::move(e_vertShader), *std::move(e_fragShader)
    };
}

ShaderProgramInstance::ShaderProgramInstance(const GLuint id, Shader&& vertShader, Shader&& fragShader)
    : m_id(id), m_vertShader(std::move(vertShader)), m_fragShader(std::move(fragShader))
{
}

auto ShaderProgramInstance::use() const -> void
{
    glUseProgram(m_id);
}

auto ShaderProgramInstance::setBool(const std::string_view& name, const bool value) -> void
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_bools, &nullTerminated))
    {
        glUniform1i(glGetUniformLocation(m_id, nullTerminated->c_str()), static_cast<GLint>(value));
    }
}

void ShaderProgramInstance::setInt(const std::string_view& name, const GLint value)
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_ints, &nullTerminated))
    {
        glUniform1i(glGetUniformLocation(m_id, nullTerminated->c_str()), value);
    }
}

void ShaderProgramInstance::setUint(const std::string_view& name, const GLuint value)
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_uints, &nullTerminated))
    {
        glUniform1ui(glGetUniformLocation(m_id, nullTerminated->c_str()), value);
    }
}

auto ShaderProgramInstance::setFloat(const std::string_view& name, const float value) -> void
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_floats, &nullTerminated))
    {
        glUniform1f(glGetUniformLocation(m_id, nullTerminated->c_str()), value);
    }
}

auto ShaderProgramInstance::setVec3(const std::string_view& name, const ft_glm::vec3& value) -> void
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_vec3s, &nullTerminated))
    {
        glUniform3f(glGetUniformLocation(m_id, nullTerminated->c_str()), value.x, value.y, value.z);
    }
}

auto ShaderProgramInstance::setVec4(const std::string_view& name, const ft_glm::vec4& value) -> void
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_vec4s, &nullTerminated))
    {
        glUniform4f(glGetUniformLocation(m_id, nullTerminated->c_str()), value.x, value.y, value.z, value.w);
    }
}

auto ShaderProgramInstance::setMat4(const std::string_view& name, const ft_glm::mat4& value) -> void
{
    const std::string* nullTerminated;
    if (storeUniformValue(name, value, m_mat4s, &nullTerminated))
    {
        glUniformMatrix4fv(glGetUniformLocation(m_id, nullTerminated->c_str()), 1, GL_FALSE, ft_glm::value_ptr(value));
    }
}

auto ShaderProgramInstance::linkProgram(const GLuint id) -> Expected<void, std::string>
{
    int success;

    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[1024];
        GLsizei infoLength;
        glGetShaderInfoLog(id, 1024, &infoLength, infoLog);
        glDeleteShader(id);
        return Unexpected(std::string(infoLog, infoLength));
    }

    return {};
}
