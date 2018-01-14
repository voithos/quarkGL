#ifndef QUARKGL_WINDOW_H_
#define QUARKGL_WINDOW_H_

#include <functional>
#include <memory>

// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <qrk/camera.h>
#include <qrk/shader.h>
#include <qrk/shared.h>

namespace qrk {

class WindowException : public QuarkException {
  using QuarkException::QuarkException;
};

constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 600;
constexpr char const* DEFAULT_TITLE = "quarkGL";

enum class EscBehavior {
  NONE,
  TOGGLE_MOUSE_CAPTURE,
  CLOSE,
};

class Window : public UniformSource {
 private:
  GLFWwindow* window_;
  bool depthTestEnabled_ = false;
  bool stencilTestEnabled_ = false;
  bool blendingEnabled_ = false;

  float lastTime_ = 0.0f;
  float deltaTime_ = 0.0f;
  glm::vec4 clearColor_ = DEFAULT_CLEAR_COLOR;

  EscBehavior escBehavior_ = EscBehavior::NONE;
  bool resizeUpdatesEnabled_ = false;
  bool keyInputEnabled_ = false;
  bool scrollInputEnabled_ = false;
  bool mouseMoveInputEnabled_ = false;

  void processInput(float deltaTime);
  void keyCallback(int key, int scancode, int action, int mods);
  void scrollCallback(double xoffset, double yoffset);
  void mouseMoveCallback(double xpos, double ypos);
  void framebufferSizeCallback(GLFWwindow* window, int width, int height);

  std::shared_ptr<Camera> boundCamera_ = nullptr;
  std::shared_ptr<CameraControls> boundCameraControls_ = nullptr;

 public:
  Window(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT,
         const char* title = DEFAULT_TITLE, bool fullscreen = false);
  ~Window();
  GLFWwindow* getGlfwRef() { return window_; }

  void activate();

  // TODO: Consider extracting depth test logic.
  void enableDepthTest() {
    glEnable(GL_DEPTH_TEST);
    depthTestEnabled_ = true;
  }
  void disableDepthTest() {
    glDisable(GL_DEPTH_TEST);
    depthTestEnabled_ = false;
  }

  // TODO: Consider extracting stencil logic out to a separate class.
  void enableStencilTest() {
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    stencilTestEnabled_ = true;
  }
  void disableStencilTest() {
    glDisable(GL_STENCIL_TEST);
    stencilTestEnabled_ = false;
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

  // TODO: Consider extracting blending logic.
  void enableBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  void disableBlending() { glDisable(GL_BLEND); }

  void enableCulling() { glEnable(GL_CULL_FACE); }
  void disableCulling() { glDisable(GL_CULL_FACE); }

  void updateUniforms(Shader& shader);

  ScreenSize getSize();
  void setSize(int width, int height);
  void enableResizeUpdates();
  void disableResizeUpdates();

  glm::vec4 getClearColor() { return clearColor_; }
  void setClearColor(glm::vec4 color) { clearColor_ = color; }

  void makeFullscreen();
  void makeWindowed();

  EscBehavior getEscBehavior() { return escBehavior_; }
  void setEscBehavior(EscBehavior behavior) { escBehavior_ = behavior; }

  void enableKeyInput();
  void disableKeyInput();
  void enableScrollInput();
  void disableScrollInput();
  void enableMouseMoveInput();
  void disableMouseMoveInput();

  void enableMouseCapture();
  void disableMouseCapture();

  void bindCamera(std::shared_ptr<Camera> camera);
  void bindCameraControls(std::shared_ptr<CameraControls> cameraControls);

  void loop(std::function<void(float)> callback);

  // TODO: Allow setting window icon.
};
}  // namespace qrk

#endif
