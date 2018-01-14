#ifndef QUARKGL_CORE_H_
#define QUARKGL_CORE_H_

// OpenGL included by glad.
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace qrk {
void init();
void terminate();
void enableGlfwErrorLogging();
void disableGlfwErrorLogging();
float time();
}  // namespace qrk

#endif
