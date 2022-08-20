#include <qrk/shader_compiler.h>
#include <qrk/shader_loader.h>

namespace qrk {
namespace {
inline const char* shaderTypeToString(ShaderType type) {
  switch (type) {
    case ShaderType::VERTEX:
      return "VERTEX";
    case ShaderType::FRAGMENT:
      return "FRAGMENT";
    case ShaderType::GEOMETRY:
      return "GEOMETRY";
    case ShaderType::COMPUTE:
      return "COMPUTE";
  }
  throw QuarkException("ERROR::SHADER::INVALID_SHADER_TYPE\n" +
                       std::to_string(static_cast<int>(type)));
}

inline const GLenum shaderTypeToGlShaderType(ShaderType type) {
  switch (type) {
    case ShaderType::VERTEX:
      return GL_VERTEX_SHADER;
    case ShaderType::FRAGMENT:
      return GL_FRAGMENT_SHADER;
    case ShaderType::GEOMETRY:
      return GL_GEOMETRY_SHADER;
    case ShaderType::COMPUTE:
      return GL_COMPUTE_SHADER;
  }
  throw QuarkException("ERROR::SHADER::INVALID_SHADER_TYPE\n" +
                       std::to_string(static_cast<int>(type)));
}
}  // namespace

unsigned int ShaderCompiler::loadAndCompileShader(
    const ShaderSource& shaderSource, const ShaderType type) {
  ShaderLoader shaderLoader(&shaderSource, type);
  std::string shaderString = shaderLoader.load();
  const char* resolvedSource = shaderString.c_str();
  unsigned int shaderId = compileShader(resolvedSource, type);
  shaders_.push_back(shaderId);
  return shaderId;
}

unsigned int ShaderCompiler::linkShaderProgram() {
  int success;
  char infoLog[512];

  // Create and link shader program.
  unsigned int shaderProgram = glCreateProgram();

  // Add the compiled shaders into the program.
  for (unsigned int shaderId : shaders_) {
    glAttachShader(shaderProgram, shaderId);
  }
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    throw ShaderCompilerException("ERROR::SHADER_COMPILER::LINKING_FAILED\n" +
                                  std::string(infoLog));
  }

  // Delete shaders now that they're linked.
  for (unsigned int shaderId : shaders_) {
    glDeleteShader(shaderId);
  }
  shaders_.clear();
  return shaderProgram;
}

unsigned int ShaderCompiler::compileShader(const char* shaderSource,
                                           const ShaderType type) {
  unsigned int shader;
  int success;
  char infoLog[512];

  // Compile shader.
  GLenum glShaderType = shaderTypeToGlShaderType(type);
  shader = glCreateShader(glShaderType);

  glShaderSource(shader, 1, &shaderSource, nullptr);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, nullptr, infoLog);
    std::string typeString(shaderTypeToString(type));
    throw ShaderCompilerException(
        "ERROR::SHADER_COMPILER::" + typeString + "::COMPILATION_FAILED\n" +
        std::string(infoLog) + "\n\nShader source:\n" +
        std::string(shaderSource));
  }
  return shader;
}

}  // namespace qrk
