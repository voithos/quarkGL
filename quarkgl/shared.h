#ifndef QUARKGL_SHARED_H_
#define QUARKGL_SHARED_H_

#include <stdexcept>

#include <glm/glm.hpp>

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

enum class ShaderType {
  VERTEX,
  FRAGMENT,
};

inline const char* shaderTypeToString(ShaderType type) {
  switch (type) {
    case ShaderType::VERTEX:
      return "VERTEX";
    case ShaderType::FRAGMENT:
      return "FRAGMENT";
  }
  throw QuarkException("ERROR::SHADER::INVALID_SHADER_TYPE\n" +
                       std::to_string(static_cast<int>(type)));
}

}  // namespace qrk

#endif
