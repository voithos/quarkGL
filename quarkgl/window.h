#ifndef QUARKGL_WINDOW_H_
#define QUARKGL_WINDOW_H_

#include <functional>

// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <qrk/shared.h>

namespace qrk {

class WindowException : public QuarkException {
  using QuarkException::QuarkException;
};

constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 600;
constexpr char const* DEFAULT_TITLE = "quarkGL";
const glm::vec4 DEFAULT_CLEAR_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

struct WindowSize {
  int width;
  int height;
};

class Window {
 private:
  GLFWwindow* window_;
  bool depthTestEnabled_ = false;
  bool stencilTestEnabled_ = false;

  float lastTime_ = 0.0f;
  float deltaTime_ = 0.0f;
  glm::vec4 clearColor_ = DEFAULT_CLEAR_COLOR;

 public:
  Window(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT,
         const char* title = DEFAULT_TITLE, bool fullscreen = false);
  ~Window();
  GLFWwindow* getGlfwRef() { return window_; }

  void activate();

  void enableDepthTest() {
    if (!depthTestEnabled_) {
      glEnable(GL_DEPTH_TEST);
      depthTestEnabled_ = true;
    }
  }
  void disableDepthTest() {
    if (depthTestEnabled_) {
      glDisable(GL_DEPTH_TEST);
      depthTestEnabled_ = false;
    }
  }

  // TODO: Consider extracting stencil logic out to a separate class.
  void enableStencilTest() {
    if (!stencilTestEnabled_) {
      glEnable(GL_STENCIL_TEST);
      glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
      stencilTestEnabled_ = true;
    }
  }
  void disableStencilTest() {
    if (stencilTestEnabled_) {
      glDisable(GL_STENCIL_TEST);
      stencilTestEnabled_ = false;
    }
  }

  void enableStencilUpdates() { glStencilMask(0xFF); }
  void disableStencilUpdates() { glStencilMask(0x00); }

  void stencilAlwaysDraw() { setStencilFunc(GL_ALWAYS); }
  void stencilDrawWhenMatching() { setStencilFunc(GL_EQUAL); }
  void stencilDrawWhenNotMatching() { setStencilFunc(GL_NOTEQUAL); }
  void setStencilFunc(GLenum func) {
    // Set the stencil test to use the given `func` when comparing for fragment
    // liveness.
    glStencilFunc(func, 1, 0xFF);
  }

  WindowSize getSize();
  void setSize(int width, int height);
  glm::vec4 getClearColor() { return clearColor_; }
  void setClearColor(glm::vec4 color) { clearColor_ = color; }

  void makeFullscreen();
  void makeWindowed();

  void loop(std::function<void(float)> callback);

  // TODO: Allow setting window icon.
};
}  // namespace qrk

#endif
