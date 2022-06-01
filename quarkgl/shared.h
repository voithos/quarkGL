#ifndef QUARKGL_SHARED_H_
#define QUARKGL_SHARED_H_

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <stdexcept>

namespace qrk {
class QuarkException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

// TODO: Consider moving some of these around.
const glm::vec4 DEFAULT_CLEAR_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

struct ScreenSize {
  int width;
  int height;
};

struct ShaderSource {
  inline explicit ShaderSource(const char* value) : value(value) {}
  const char* value;
  virtual bool isPath() const = 0;
};

struct ShaderInline : ShaderSource {
  using ShaderSource::ShaderSource;
  bool isPath() const { return false; }
};

struct ShaderPath : ShaderSource {
  using ShaderSource::ShaderSource;
  bool isPath() const { return true; }
};

enum class ShaderType {
  VERTEX,
  FRAGMENT,
  GEOMETRY,
};

inline const char* shaderTypeToString(ShaderType type) {
  switch (type) {
    case ShaderType::VERTEX:
      return "VERTEX";
    case ShaderType::FRAGMENT:
      return "FRAGMENT";
    case ShaderType::GEOMETRY:
      return "GEOMETRY";
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
  }
  throw QuarkException("ERROR::SHADER::INVALID_SHADER_TYPE\n" +
                       std::to_string(static_cast<int>(type)));
}

}  // namespace qrk

#endif
