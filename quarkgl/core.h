#ifndef QUARKGL_CORE_H_
#define QUARKGL_CORE_H_

// OpenGL included by glad, so disable GLFW's attempts to load extensions.
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace qrk {
void init();
void terminate();
void enableGlfwErrorLogging();
void disableGlfwErrorLogging();
void initGlErrorLogging();
void enableGlErrorLogging();
void disableGlErrorLogging();
float time();
}  // namespace qrk

#endif
