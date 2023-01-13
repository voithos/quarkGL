#include <qrk/debug.h>

namespace qrk {

qrk::DebugGroup::DebugGroup(const char* name) {
  glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, name);
}

DebugGroup::~DebugGroup() { glPopDebugGroup(); }

}  // namespace qrk