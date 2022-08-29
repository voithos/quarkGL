#ifndef QUARKGL_WINDOW_H_
#define QUARKGL_WINDOW_H_

// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>
// clang-format on

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <qrk/camera.h>
#include <qrk/exceptions.h>
#include <qrk/screen.h>
#include <qrk/shader.h>

#include <functional>
#include <glm/glm.hpp>
#include <memory>
#include <tuple>
#include <vector>

namespace qrk {

class WindowException : public QuarkException {
  using QuarkException::QuarkException;
};

constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 600;
constexpr char const* DEFAULT_TITLE = "quarkGL";
const glm::vec4 DEFAULT_CLEAR_COLOR = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

enum class EscBehavior {
  NONE,
  TOGGLE_MOUSE_CAPTURE,
  CLOSE,
};

class Window : public UniformSource {
 public:
  Window(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT,
         const char* title = DEFAULT_TITLE, bool fullscreen = false,
         int samples = 0);
  virtual ~Window();
  GLFWwindow* getGlfwRef() const { return window_; }

  // TODO: Should this be called something different, and 'activate' be used for
  // setViewport?
  void activate();

  void setViewport() {
    ScreenSize size = getSize();
    glViewport(0, 0, size.width, size.height);
  }

  void enableVsync() {
    activate();
    glfwSwapInterval(1);
  }
  void disableVsync() {
    activate();
    glfwSwapInterval(0);
  }

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
    // Only replace the value in the stencil buffer if both the stencil and
    // depth test pass.
    glStencilOp(/*sfail=*/GL_KEEP, /*dpfail=*/GL_KEEP, /*dppass=*/GL_REPLACE);
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
    glStencilFunc(func, /*ref=*/1, /*mask=*/0xFF);
  }

  // TODO: Consider extracting blending logic.
  void enableBlending() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  void disableBlending() { glDisable(GL_BLEND); }

  void enableFaceCull() { glEnable(GL_CULL_FACE); }
  void disableFaceCull() { glDisable(GL_CULL_FACE); }

  void cullFrontFaces() { glCullFace(GL_FRONT); }
  void cullBackFaces() { glCullFace(GL_BACK); }

  void updateUniforms(Shader& shader);

  ScreenSize getSize() const;
  void setSize(int width, int height);
  void enableResizeUpdates();
  void disableResizeUpdates();

  unsigned int getFrameCount() const { return frameCount_; }
  glm::vec4 getClearColor() const { return clearColor_; }
  void setClearColor(glm::vec4 color) { clearColor_ = color; }

  void makeFullscreen();
  void makeWindowed();

  EscBehavior getEscBehavior() const { return escBehavior_; }
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

  // TODO: Add a way to remove handlers.
  void addKeyPressHandler(int glfwKey, std::function<void(int)> handler);

  void loop(std::function<void(float)> callback);

  // TODO: Allow setting window icon.

 private:
  void processInput(float deltaTime);
  void keyCallback(int key, int scancode, int action, int mods);
  void scrollCallback(double xoffset, double yoffset);
  void mouseMoveCallback(double xpos, double ypos);
  void framebufferSizeCallback(GLFWwindow* window, int width, int height);

  GLFWwindow* window_;
  bool depthTestEnabled_ = false;
  bool stencilTestEnabled_ = false;

  float lastTime_ = 0.0f;
  float deltaTime_ = 0.0f;
  unsigned int frameCount_ = 0;
  glm::vec4 clearColor_ = DEFAULT_CLEAR_COLOR;

  EscBehavior escBehavior_ = EscBehavior::NONE;
  bool resizeUpdatesEnabled_ = false;
  bool keyInputEnabled_ = false;
  bool scrollInputEnabled_ = false;
  bool mouseMoveInputEnabled_ = false;
  std::vector<std::tuple<int, std::function<void(int)>>> keyPressHandlers_;

  std::shared_ptr<Camera> boundCamera_ = nullptr;
  std::shared_ptr<CameraControls> boundCameraControls_ = nullptr;
};
}  // namespace qrk

#endif
