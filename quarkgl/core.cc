#include <qrk/core.h>

namespace qrk {

/** Allows init() to be idempotent. */
bool isInitialized = false;

void init() {
  if (!isInitialized) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    isInitialized = true;
  }
}

void terminate() {
  if (isInitialized) {
    glfwTerminate();
    isInitialized = false;
  }
}

float time() { return glfwGetTime(); }
}  // namespace qrk
