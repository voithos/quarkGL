#ifndef QUARKGL_SHARED_H_
#define QUARKGL_SHARED_H_

#include <stdexcept>

namespace qrk {
class QuarkException : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

// TODO: Pull to a common shader helper file.
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
    default:
      throw QuarkException("ERROR::SHADER::INVALID_SHADER_TYPE\n" +
                           std::to_string(static_cast<int>(type)));
  }
}

}  // namespace qrk

#endif
