#include <qrk/window.h>

namespace qrk {
Window::Window(int width, int height, const char* title, bool fullscreen,
               int samples) {
  qrk::init();

  if (samples > 0) {
    glfwWindowHint(GLFW_SAMPLES, samples);
  }

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

  // Enable multisampling if needed.
  if (samples > 0) {
    glEnable(GL_MULTISAMPLE);
  }

  // A few options are enabled by default.
  enableDepthTest();
  enableResizeUpdates();
  enableKeyInput();
  enableScrollInput();
  enableMouseMoveInput();
  enableMouseButtonInput();
}

Window::~Window() {
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }
  qrk::terminate();
}

void Window::activate() { glfwMakeContextCurrent(window_); }

void Window::updateUniforms(Shader& shader) {
  shader.setFloat("qrk_deltaTime", deltaTime_);

  ScreenSize size = getSize();
  shader.setInt("qrk_windowWidth", size.width);
  shader.setInt("qrk_windowHeight", size.height);
}

ScreenSize Window::getSize() const {
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
  glfwSetFramebufferSizeCallback(window_, callback);
  resizeUpdatesEnabled_ = true;
}

void Window::disableResizeUpdates() {
  if (!resizeUpdatesEnabled_) return;
  glfwSetFramebufferSizeCallback(window_, nullptr);
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

void Window::enableScrollInput() {
  if (scrollInputEnabled_) return;
  auto callback = [](GLFWwindow* window, double xoffset, double yoffset) {
    auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->scrollCallback(xoffset, yoffset);
  };
  glfwSetScrollCallback(window_, callback);
  scrollInputEnabled_ = true;
}

void Window::disableScrollInput() {
  if (!scrollInputEnabled_) return;
  glfwSetScrollCallback(window_, nullptr);
  scrollInputEnabled_ = false;
}

void Window::enableMouseMoveInput() {
  if (mouseMoveInputEnabled_) return;
  auto callback = [](GLFWwindow* window, double xpos, double ypos) {
    auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->mouseMoveCallback(xpos, ypos);
  };
  glfwSetCursorPosCallback(window_, callback);
  mouseMoveInputEnabled_ = true;
}

void Window::disableMouseMoveInput() {
  if (!mouseMoveInputEnabled_) return;
  glfwSetCursorPosCallback(window_, nullptr);
  mouseMoveInputEnabled_ = false;
}

void Window::enableMouseButtonInput() {
  if (mouseButtonInputEnabled_) return;
  auto callback = [](GLFWwindow* window, int button, int action, int mods) {
    auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    self->mouseButtonCallback(button, action, mods);
  };
  glfwSetMouseButtonCallback(window_, callback);
  mouseButtonInputEnabled_ = true;
}

void Window::disableMouseButtonInput() {
  if (!mouseButtonInputEnabled_) return;
  glfwSetMouseButtonCallback(window_, nullptr);
  mouseButtonInputEnabled_ = false;
}

void Window::framebufferSizeCallback(GLFWwindow* window, int width,
                                     int height) {
  glViewport(0, 0, width, height);

  if (boundCamera_) {
    boundCamera_->setAspectRatio(width / static_cast<float>(height));
  }
  if (boundCameraControls_) {
    boundCameraControls_->resizeWindow(width, height);
  }
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

void Window::processInput(float deltaTime) {
  if (boundCameraControls_) {
    boundCameraControls_->processInput(window_, *boundCamera_, deltaTime);
  }
}

void Window::keyCallback(int key, int scancode, int action, int mods) {
  if (keyInputPaused_) return;

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
    } else if (escBehavior_ == EscBehavior::UNCAPTURE_MOUSE_OR_CLOSE) {
      auto inputMode = glfwGetInputMode(window_, GLFW_CURSOR);
      if (inputMode == GLFW_CURSOR_DISABLED) {
        disableMouseCapture();
      } else {
        // Close since mouse is not captured.
        glfwSetWindowShouldClose(window_, true);
      }
    }
  }

  // Run handlers.
  if (action == GLFW_PRESS) {
    for (auto pair : keyPressHandlers_) {
      int glfwKey;
      std::function<void(int)> handler;
      std::tie(glfwKey, handler) = pair;

      if (key == glfwKey) {
        handler(mods);
      }
    }
  }
}

void Window::scrollCallback(double xoffset, double yoffset) {
  if (mouseInputPaused_) return;

  if (boundCameraControls_) {
    boundCameraControls_->scroll(*boundCamera_, xoffset, yoffset);
  }
}

void Window::mouseMoveCallback(double xpos, double ypos) {
  if (mouseInputPaused_) return;

  if (boundCameraControls_) {
    boundCameraControls_->mouseMove(*boundCamera_, xpos, ypos);
  }
}

void Window::mouseButtonCallback(int button, int action, int mods) {
  if (mouseInputPaused_) return;

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    if (mouseButtonBehavior_ == MouseButtonBehavior::CAPTURE_MOUSE) {
      enableMouseCapture();
    }
  }

  // Run handlers.
  if (action == GLFW_PRESS) {
    for (auto pair : mouseButtonHandlers_) {
      int glfwMouseButton;
      std::function<void(int)> handler;
      std::tie(glfwMouseButton, handler) = pair;

      if (button == glfwMouseButton) {
        handler(mods);
      }
    }
  }
}

void Window::addKeyPressHandler(int glfwKey, std::function<void(int)> handler) {
  keyPressHandlers_.push_back(std::make_tuple(glfwKey, handler));
}
void Window::addMouseButtonHandler(int glfwMouseButton,
                                   std::function<void(int)> handler) {
  mouseButtonHandlers_.push_back(std::make_tuple(glfwMouseButton, handler));
}

void Window::enableMouseCapture() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::disableMouseCapture() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::bindCamera(std::shared_ptr<Camera> camera) {
  boundCamera_ = camera;
  boundCamera_->setAspectRatio(getSize());
}

void Window::bindCameraControls(
    std::shared_ptr<CameraControls> cameraControls) {
  if (!boundCamera_) {
    throw WindowException(
        "ERROR::WINDOW::BIND_CAMERA_CONTROLS_FAILED\n"
        "Camera must be bound before camera controls.");
  }
  boundCameraControls_ = cameraControls;
  ScreenSize size = getSize();
  boundCameraControls_->resizeWindow(size.width, size.height);
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

    qrkCheckForGlError();

    glfwSwapBuffers(window_);
    glfwPollEvents();

    frameCount_++;
  }
}

}  // namespace qrk
