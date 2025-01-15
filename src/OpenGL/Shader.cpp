//
// Created by loumarti on 1/15/25.
//

#include "Shader.h"

Shader::Shader(const std::string &filepath) : m_filepath(filepath), m_rendererID(0) {
  ShaderProgramSource sources = _parseShader(filepath);

  // [!] Debug // TODo delete or use with debug flag
  std::cout << sources << std::endl;

  m_rendererID = _createShader(sources.vertex, sources.fragment);
}

// Set uniforms // [?] TODO ? Put in .h as inline ?
void Shader::setUniform1i(const std::string &name, int value) {
  glUniform1i(_getUniformLocation(name), value);
}

void Shader::setUniform1f(const std::string &name, float value) {
  glUniform1f(_getUniformLocation(name), value);
}

void Shader::setUniform4f(const std::string &name, float v0, float v1, float v2, float v3) {
  glUniform4f(_getUniformLocation(name), v0, v1, v2, v3);
}

void Shader::setUniformMat4f(const std::string &name, const glm::mat4 &matrix) {
  glUniformMatrix4fv(_getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}


int Shader::_getUniformLocation(const std::string &name) {
  if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
    return m_uniformLocationCache[name];

  int location = -1;
   location = glGetUniformLocation(m_rendererID, name.c_str());
   if (location < 0)
    std::cout << "[warning] uniform location of '" << name << "' can't be found" << std::endl;

   m_uniformLocationCache[name] = location;
   return location;
}


ShaderProgramSource Shader::_parseShader(const std::string &filepath) {
  std::ifstream stream(filepath);

  enum class ShaderType {
    NONE = -1, VERTEX = 0, FRAGMENT = 1
  };

  std::string line;
  std::stringstream ss[2];
  ShaderType type = ShaderType::NONE;
  while (getline(stream, line)) {
    if (line.find("//") != std::string::npos) {
        continue;
    }
    if (line.find("#shader") != std::string::npos) {
      if (line.find("vertex") != std::string::npos)
        type = ShaderType::VERTEX;
      else if (line.find("fragment") != std::string::npos)
        type = ShaderType::FRAGMENT;
    } else {
      if (type != ShaderType::NONE) {
        ss[static_cast<int>(type)] << line << '\n';
      }
    }
  }

  return { ss[0].str(), ss[1].str() };
}

// https://docs.gl/gl4/glShaderSource
unsigned int Shader::_compileShader(unsigned int type, const std::string &source) {
  unsigned int id = glCreateShader(type);

  std::string sourceDup = source;
  const char *src = sourceDup.c_str();

  glShaderSource(id, 1 , &src, nullptr);
  glCompileShader(id);

  // Handle errors
  int result;
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    char *message = (char *)alloca(length * sizeof(char));
    glGetShaderInfoLog(id, length, &length, message);
    std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
    std::cout << message << std::endl;
  }

  return id;
}


// Shader are like files to compiles. Once done we can delete it, more or less like *.o files)
unsigned int Shader::_createShader(const std::string &vertexShader, const std::string &fragmentShadder) {
  unsigned int program;
  program = glCreateProgram();
  unsigned int vs = _compileShader(GL_VERTEX_SHADER, vertexShader);
  unsigned int fs = _compileShader(GL_FRAGMENT_SHADER, fragmentShadder);

  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);

  glDeleteShader(vs);
  glDeleteShader(fs);

  glDetachShader(program, vs);
  glDetachShader(program, fs);

  return program;
}

/* ----- Displaying Tools ----- */

std::ostream &operator<<(std::ostream &o, const ShaderProgramSource &righty) {
    o << "\033[1;36m#Vertex shader\033[0m" << std::endl;
    o << righty.vertex << std::endl;
    o << "\033[1;36m#Fragment shader\033[0m" << std::endl;
    o << righty.fragment << std::endl;
    return o;
}