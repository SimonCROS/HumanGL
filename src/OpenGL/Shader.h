#ifndef SHADER_H
#define SHADER_H

#include <string_view>

#include "glad/gl.h"

namespace Shader
{
    auto createShader(const std::string_view& code, GLenum type) -> GLuint;
    auto destroyShader(GLuint shader) -> void;
    auto tryGetShaderCode(const std::string& path, std::string& output) -> bool;
}

#endif
