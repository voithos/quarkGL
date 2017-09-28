#include <qrk/shader.h>

namespace qrk {

std::string readFile(const char* path) {
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  file.open(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
  // Read file contents.
  std::string vertexSourceString;
  std::string fragmentSourceString;

  try {
    vertexSourceString = readFile(vertexPath);
    fragmentSourceString = readFile(fragmentPath);
  } catch (std::ifstream::failure e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
  }

  const char* vertexShaderSource = vertexSourceString.c_str();
  const char* fragmentShaderSource = fragmentSourceString.c_str();

  unsigned int vertexShader, fragmentShader;
  int success;
  char infoLog[512];

  // Compile vertex shader.
  vertexShader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
  glCompileShader(vertexShader);

  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  // Compile fragment shader.
  fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
  glCompileShader(fragmentShader);

  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }

  // Create and link shader program.
  shaderProgram_ = glCreateProgram();

  glAttachShader(shaderProgram_, vertexShader);
  glAttachShader(shaderProgram_, fragmentShader);
  glLinkProgram(shaderProgram_);

  glGetProgramiv(shaderProgram_, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram_, 512, nullptr, infoLog);
    std::cout << "ERROR::SHADER::LINKING_FAILED\n" << infoLog << std::endl;
  }

  // Delete shaders now that they're linked.
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

void Shader::activate() { glUseProgram(shaderProgram_); }
void Shader::deactive() { glUseProgram(0); }

void Shader::setBool(const char* name, bool value) {
  activate();
  glUniform1i(glGetUniformLocation(shaderProgram_, name),
              static_cast<int>(value));
}

void Shader::setInt(const char* name, int value) {
  activate();
  glUniform1i(glGetUniformLocation(shaderProgram_, name), value);
}

void Shader::setFloat(const char* name, float value) {
  activate();
  glUniform1f(glGetUniformLocation(shaderProgram_, name), value);
}

void Shader::setVec3(const char* name, const glm::vec3& vector) {
  activate();
  glUniform3fv(glGetUniformLocation(shaderProgram_, name), 1,
               glm::value_ptr(vector));
}

void Shader::setVec3(const char* name, float v0, float v1, float v2) {
  activate();
  glUniform3f(glGetUniformLocation(shaderProgram_, name), v0, v1, v2);
}

void Shader::setMat4(const char* name, const glm::mat4& matrix) {
  activate();
  glUniformMatrix4fv(glGetUniformLocation(shaderProgram_, name), 1, GL_FALSE,
                     glm::value_ptr(matrix));
}
}  // namespace qrk
