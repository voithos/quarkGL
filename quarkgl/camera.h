#ifndef QUARKGL_CAMERA_H_
#define QUARKGL_CAMERA_H_

// clang-format off
// Must precede glfw/glad, to include OpenGL functions.
#include <qrk/core.h>
// clang-format on

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <qrk/light.h>
#include <qrk/screen.h>
#include <qrk/shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace qrk {

enum class CameraDirection {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
  UP,
  DOWN,
};

constexpr float DEFAULT_YAW = 270.0f;
constexpr float DEFAULT_PITCH = 0.0f;
constexpr float DEFAULT_SPEED = 2.5f;
constexpr float DEFAULT_SENSITIVITY = 0.1f;
constexpr float DEFAULT_FOV = 45.0f;
constexpr float DEFAULT_NEAR = 0.1f;
constexpr float DEFAULT_FAR = 100.0f;
constexpr float DEFAULT_ASPECT_RATIO = 4.0f / 3.0f;

constexpr float MIN_FOV = 1.0f;
constexpr float MAX_FOV = 45.0f;

class Camera : public UniformSource, public ViewSource {
 public:
  // Constructs a new Camera. Angular values should be provided in degrees.
  Camera(glm::vec3 position = glm::vec3(0.0f),
         glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
         float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH,
         float speed = DEFAULT_SPEED, float sensitivity = DEFAULT_SENSITIVITY,
         float fov = DEFAULT_FOV, float aspectRatio = DEFAULT_ASPECT_RATIO,
         float near = DEFAULT_NEAR, float far = DEFAULT_FAR);
  virtual ~Camera() = default;

  glm::vec3 getPosition() const { return position_; }
  glm::vec3 getFront() const { return front_; }
  float getFov() const { return fov_; }

  // TODO: Allow programmatic position modification.
  // TODO: Add the rest of the getters/setters for fields.
  float getAspectRatio() const { return aspectRatio_; }
  void setAspectRatio(float aspectRatio) { aspectRatio_ = aspectRatio; }
  void setAspectRatio(ScreenSize size) {
    aspectRatio_ = size.width / static_cast<float>(size.height);
  }

  glm::mat4 getViewTransform() const override;
  glm::mat4 getPerspectiveTransform() const;

  void updateUniforms(Shader& shader) override;

  // TODO: Move the following methods to FlyCameraControl?

  // Moves the camera in the given direction based on the current speed.
  void move(CameraDirection direction, float deltaTime);
  // Rotates the camera based on the current sensitivity. If constrainPitch is
  // true, pitch is clamped when near the up and down directions.
  void rotate(float xoffset, float yoffset, bool constrainPitch = true);
  // Changes the zoom level of the FoV by the given offset.
  void zoom(float offset);

 private:
  void updateCameraVectors();

  glm::vec3 position_;
  glm::vec3 front_;
  glm::vec3 up_;
  glm::vec3 right_;
  glm::vec3 worldUp_;

  float yaw_;
  float pitch_;
  float speed_;
  float sensitivity_;
  float fov_;

  float aspectRatio_;
  float near_;
  float far_;
};

// Interface for camera controllers.
class CameraControls {
 public:
  virtual void resizeWindow(int width, int height) = 0;
  virtual void scroll(Camera& camera, double xoffset, double yoffset) = 0;
  virtual void mouseMove(Camera& camera, double xpos, double ypos) = 0;
  virtual void processInput(GLFWwindow* window, Camera& camera,
                            float deltaTime) = 0;
};

// Camera controls that implement a fly mode, similar to DCC tools.
class FlyCameraControls : public CameraControls {
 public:
  virtual ~FlyCameraControls() = default;
  void resizeWindow(int width, int height);
  void scroll(Camera& camera, double xoffset, double yoffset);
  void mouseMove(Camera& camera, double xpos, double ypos);
  void processInput(GLFWwindow* window, Camera& camera, float deltaTime);

 private:
  bool initialized_ = false;
  int width_;
  int height_;

  float lastX_;
  float lastY_;
  bool initialMouse_ = true;
};

}  // namespace qrk

#endif
