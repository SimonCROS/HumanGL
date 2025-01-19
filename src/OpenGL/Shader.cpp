#include <fstream>
#include <sstream>

#include "glad/gl.h"
// #include "logger.hpp"
#include "Shader.h"

namespace Shader
{
    static auto tryCreateShader(const GLenum type, const std::string_view& code, GLuint* id) -> bool
    {
        const GLuint shaderId = glCreateShader(type);

        if (shaderId == 0)
        {
            return false;
        }

        const auto str = code.data();
        const auto length = static_cast<GLint>(code.size());
        glShaderSource(shaderId, 1, &str, &length);
        *id = shaderId;
        return true;
    }

    static auto compileShader(const GLuint id) -> bool
    {
        int success;

        glCompileShader(id);
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            // char infoLog[1024];
            // glGetShaderInfoLog(id, 1024, NULL, infoLog);
            // logError2(FTRUN_SHADER, FTRUN_COMPILATION_FAILED, infoLog);
            return false;
        }

        return true;
    }

    auto createShader(const std::string_view& code, const GLenum type) -> GLuint
    {
        GLuint id;
        if (!tryCreateShader(type, code, &id))
        {
            return 0;
        }

        if (!compileShader(id))
        {
            glDeleteShader(id);
            return 0;
        }

        return id;
    }

    auto destroyShader(const GLuint shader) -> void
    {
        glDeleteShader(shader);
    }

    auto tryGetShaderCode(const std::string& path, std::string* code) -> bool
    {
        std::ifstream file;

        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            std::stringstream stream;
            file.open(path.c_str());
            stream << file.rdbuf();
            file.close();

            *code = stream.str();
            return true;
        }
        catch (std::ifstream::failure& e)
        {
            // logError2(FTRUN_SHADER, FTRUN_FILE_NOT_SUCCESSFULLY_READ, "(" << path << ") " << e.what());
            return false;
        }
    }
}
