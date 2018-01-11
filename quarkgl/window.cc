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

  enableDepthTest();
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

    // Call the loop function.
    callback(deltaTime_);

    glfwSwapBuffers(window_);
    glfwPollEvents();
  }
}

}  // namespace qrk
