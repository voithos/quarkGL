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
}

Window::~Window() {
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }
  qrk::terminate();
}

void Window::activate() { glfwMakeContextCurrent(window_); }

WindowSize Window::getSize() {
  WindowSize size;
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
}  // namespace qrk
