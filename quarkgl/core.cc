#include <qrk/core.h>

#include <iostream>

namespace qrk {

/** Allows init() to be idempotent. */
bool isInitialized = false;

/** Whether or not to automatically print glfw errors. */
bool errorPrintingEnabled = true;

void errorCallback(int error, const char* description) {
  if (errorPrintingEnabled) {
    std::cout << "GLFW ERROR: " << description << " [error code " << error
              << "]" << std::endl;
  }
}

void init() {
  if (!isInitialized) {
    glfwSetErrorCallback(errorCallback);

    glfwInit();
    // TODO: Do we need this? Move into qrk::Window?
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

void enableGlfwErrorLogging() { errorPrintingEnabled = true; }
void disableGlfwErrorLogging() { errorPrintingEnabled = false; }

float time() { return glfwGetTime(); }
}  // namespace qrk
