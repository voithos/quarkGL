#include "shader.h"

namespace qrk {

std::string readFile(const char *path) {
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  file.open(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
  // Read file contents.
  std::string vertexSourceString;
  std::string fragmentSourceString;

  try {
    vertexSourceString = readFile(vertexPath);
    fragmentSourceString = readFile(fragmentPath);
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }

  const char *vertexShaderSource = vertexSourceString.c_str();
  const char *fragmentShaderSource = fragmentSourceString.c_str();

  unsigned int vertexShader, fragmentShader;
  int success;
  char infoLog[512];

  // Compile vertex shader.
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  // Compile fragment shader.
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  // Create and link shader program.
  shaderProgram = glCreateProgram();

  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
  }

  // Delete shaders now that they're linked.
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

unsigned int Shader::getProgramId() { return shaderProgram; }

void Shader::use() { glUseProgram(shaderProgram); }

void Shader::setBool(const std::string &name, bool value) const {
  glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
  glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
  glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}

void Shader::setMatrix4f(const std::string &name, glm::mat4 &matrix) const {
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1,
                     GL_FALSE, glm::value_ptr(matrix));
}
}
