#ifndef QUARKGL_DEBUG_H_
#define QUARKGL_DEBUG_H_

// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>
// clang-format on

namespace qrk {

// RAII debugging group marker.
class DebugGroup {
 public:
  DebugGroup(const char* name);
  ~DebugGroup();
};

}  // namespace qrk

#endif
