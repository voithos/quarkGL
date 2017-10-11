#ifndef QUARKGL_SHARED_H_
#define QUARKGL_SHARED_H_

#include <stdexcept>

namespace qrk {
class QuarkException : public std::runtime_error {
  using std::runtime_error::runtime_error;
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
}

}  // namespace qrk

#endif
