#include <qrk/exceptions.h>

namespace qrk {

std::string glErrorToString(GLenum errorCode) {
  switch (errorCode) {
    case GL_NO_ERROR:
      return "NO_ERROR";
    case GL_INVALID_ENUM:
      return "INVALID_ENUM";
    case GL_INVALID_VALUE:
      return "INVALID_VALUE";
    case GL_INVALID_OPERATION:
      return "INVALID_OPERATION";
    case GL_STACK_OVERFLOW:
      return "STACK_OVERFLOW";
    case GL_STACK_UNDERFLOW:
      return "STACK_UNDERFLOW";
    case GL_OUT_OF_MEMORY:
      return "OUT_OF_MEMORY";
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      return "INVALID_FRAMEBUFFER_OPERATION";
    default:
      return "UNKNOWN";
  }
}

void checkForGlError(const char* file, int line) {
  GLenum errorCode = glGetError();
  if (errorCode != GL_NO_ERROR) {
    throw GlException("ERROR::GL::" + glErrorToString(errorCode) + "(" +
                      std::string(file) + ":" + std::to_string(line) + ")");
  }
}

}  // namespace qrk
