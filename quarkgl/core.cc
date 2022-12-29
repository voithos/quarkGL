#include <qrk/core.h>

#include <cstdio>

namespace qrk {

/** Allows init() to be idempotent. */
bool isInitialized = false;

/** Whether or not to automatically print glfw errors. */
bool glfwErrorLoggingEnabled = true;

/** Whether or not to automatically print OpenGL debug messages. */
bool glErrorLoggingEnabled = true;

void glfwErrorCallback(int error, const char* description) {
  if (glfwErrorLoggingEnabled) {
    fprintf(stderr, "GLFW ERROR: %s [error code %d]\n", description, error);
  }
}

void GLAPIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id,
                                GLenum severity, GLsizei length,
                                const GLchar* message, const void* userParam) {
  if (glErrorLoggingEnabled && type == GL_DEBUG_TYPE_ERROR) {
    fprintf(stderr, "GL ERROR: %s [error type = 0x%x, severity = 0x%x]\n",
            message, type, severity);
  }
}

void init() {
  if (!isInitialized) {
    glfwSetErrorCallback(glfwErrorCallback);

    glfwInit();
    // TODO: Do we need this? Move into qrk::Window?
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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

void enableGlfwErrorLogging() { glfwErrorLoggingEnabled = true; }
void disableGlfwErrorLogging() { glfwErrorLoggingEnabled = false; }

void initGlErrorLogging() {
  if (glErrorLoggingEnabled) {
    enableGlErrorLogging();
  } else {
    disableGlErrorLogging();
  }
}

void enableGlErrorLogging() {
  glErrorLoggingEnabled = true;
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(glDebugCallback, 0);
}
void disableGlErrorLogging() {
  glErrorLoggingEnabled = false;
  glDisable(GL_DEBUG_OUTPUT);
}

float time() { return glfwGetTime(); }
}  // namespace qrk
