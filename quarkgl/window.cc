#include <qrk/window.h>

namespace qrk {
Window::Window(int width, int height, const char* title, bool fullscreen) {
  qrk::init();

  // nullptr indicates windowed.
  GLFWmonitor* monitor = nullptr;
  if (fullscreen) {
    monitor = glfwGetPrimaryMonitor();
  }

  window_ =
      glfwCreateWindow(width, height, title, monitor, /* share */ nullptr);

  if (window_ == nullptr) {
    qrk::terminate();
    throw WindowException("ERROR::WINDOW::CREATE_FAILED");
  }

  activate();

  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    throw WindowException("ERROR::WINDOW::GLAD_INITIALIZATION_FAILED");
  }

  // Allow us to refer to the object while accessing C APIs.
  glfwSetWindowUserPointer(window_, this);

  // A few options are enabled by default.
  enableDepthTest();
  enableResizeUpdates();
  enableKeyInput();
}

Window::~Window() {
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }
  qrk::terminate();
}

void Window::activate() { glfwMakeContextCurrent(window_); }

void Window::updateUniforms(Shader& shader) {
  ScreenSize size = getSize();
  shader.setInt("qrk_windowWidth", size.width);
  shader.setInt("qrk_windowHeight", size.height);
}

ScreenSize Window::getSize() {
  ScreenSize size;
  glfwGetWindowSize(window_, &size.width, &size.height);
  return size;
}

void Window::setSize(int width, int height) {
  glfwSetWindowSize(window_, width, height);
}

void Window::enableResizeUpdates() {
  if (resizeUpdatesEnabled_) return;
  auto callback = [](GLFWwindow* window, int width, int height) {
    auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->framebufferSizeCallback(window, width, height);
  };
  glfwSetFramebufferSizeCallback(getGlfwRef(), callback);
  resizeUpdatesEnabled_ = true;
}

void Window::disableResizeUpdates() {
  if (!resizeUpdatesEnabled_) return;
  glfwSetFramebufferSizeCallback(getGlfwRef(), nullptr);
  resizeUpdatesEnabled_ = false;
}

void Window::enableKeyInput() {
  if (keyInputEnabled_) return;
  auto callback = [](GLFWwindow* window, int key, int scancode, int action,
                     int mods) {
    auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->keyCallback(key, scancode, action, mods);
  };
  glfwSetKeyCallback(window_, callback);
  keyInputEnabled_ = true;
}

void Window::disableKeyInput() {
  if (!keyInputEnabled_) return;
  glfwSetKeyCallback(window_, nullptr);
  keyInputEnabled_ = false;
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width,
                                     int height) {
  // TODO: Propagate the new aspect ratio to the camera.
  glViewport(0, 0, width, height);
}

void Window::makeFullscreen() {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  auto size = getSize();
  glfwSetWindowMonitor(window_, monitor, /* unused xpos */ 0,
                       /* unused ypos */ 0, size.width, size.height,
                       /* refreshRate */ GLFW_DONT_CARE);
}

void Window::makeWindowed() {
  auto size = getSize();
  glfwSetWindowMonitor(window_, /* monitor */ nullptr, /* xpos */ 0,
                       /* ypos */ 0, size.width, size.height,
                       /* refreshRate */ GLFW_DONT_CARE);
}

void Window::processInput(float deltaTime) {}

void Window::keyCallback(int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    if (escBehavior_ == EscBehavior::TOGGLE_MOUSE_CAPTURE) {
      auto inputMode = glfwGetInputMode(window_, GLFW_CURSOR);
      if (inputMode == GLFW_CURSOR_NORMAL) {
        enableMouseCapture();
      } else {
        disableMouseCapture();
      }
    } else if (escBehavior_ == EscBehavior::CLOSE) {
      glfwSetWindowShouldClose(window_, true);
    }
  }
}

void Window::enableMouseCapture() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::disableMouseCapture() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::loop(std::function<void(float)> callback) {
  // TODO: Add exception handling here.
  while (!glfwWindowShouldClose(window_)) {
    float currentTime = qrk::time();
    deltaTime_ = currentTime - lastTime_;
    lastTime_ = currentTime;

    // Clear the appropriate buffers.
    glClearColor(clearColor_.r, clearColor_.g, clearColor_.b, clearColor_.a);
    auto clearBits = GL_COLOR_BUFFER_BIT;
    if (depthTestEnabled_) {
      clearBits |= GL_DEPTH_BUFFER_BIT;
    }
    if (stencilTestEnabled_) {
      clearBits |= GL_STENCIL_BUFFER_BIT;
    }
    glClear(clearBits);

    // Process necessary input.
    processInput(deltaTime_);

    // Call the loop function.
    callback(deltaTime_);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

}  // namespace qrk
