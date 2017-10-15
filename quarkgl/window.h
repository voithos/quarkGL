#ifndef QUARKGL_WINDOW_H_
#define QUARKGL_WINDOW_H_

// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <qrk/shared.h>

namespace qrk {

class WindowException : public QuarkException {
  using QuarkException::QuarkException;
};

constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 600;
constexpr char const* DEFAULT_TITLE = "quarkGL";

struct WindowSize {
  int width;
  int height;
};

class Window {
 private:
  GLFWwindow* window_;

 public:
  Window(int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT,
         const char* title = DEFAULT_TITLE, bool fullscreen = false);
  ~Window();
  GLFWwindow* getGlfwRef() { return window_; }

  void activate();

  WindowSize getSize();
  void setSize(int width, int height);

  void makeFullscreen();
  void makeWindowed();

  // TODO: Allow setting window icon.
};
}  // namespace qrk

#endif
