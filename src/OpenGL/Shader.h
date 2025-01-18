//
// Created by loumarti on 1/15/25.
//

#ifndef SHADER_H
#define SHADER_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <glm/fwd.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad/gl.h"

struct ShaderProgramSource
{
    std::string vertex;
    std::string fragment;
};

class Shader
{
private:
    std::string m_filepath;
    unsigned int m_rendererID;
    std::unordered_map<std::string, int> m_uniformLocationCache;

public:
    explicit Shader(const std::string& filepath);

    ~Shader()
    {
        glDeleteProgram(m_rendererID);
    };

    void bind() const
    {
        glUseProgram(m_rendererID);
    };

    void unbind() const
    {
        glUseProgram(0);
    };

    // Set uniforms
    void setUniform1i(const std::string& name, int value);
    void setUniform1f(const std::string& name, float value);
    void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
    void setUniformMat4f(const std::string& name, const glm::mat4& matrix);

private:
    int _getUniformLocation(const std::string& name);
    ShaderProgramSource _parseShader(const std::string& filepath);
    unsigned int _compileShader(unsigned int type, const std::string& source);
    unsigned int _createShader(const std::string& vertexShader, const std::string& fragmentShader);
};

/* ----- Displaying Tools ----- */
std::ostream& operator<<(std::ostream& o, const ShaderProgramSource& righty);


#endif //SHADER_H
