#ifndef CORE_H
#define CORE_H

// OpenGL included by glad.
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace qrk {
void init();
void terminate();
}

#endif
