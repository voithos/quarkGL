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
constexpr float DEFAULT_FOV = 45.0f;
constexpr float DEFAULT_NEAR = 0.1f;
constexpr float DEFAULT_FAR = 100.0f;
constexpr float DEFAULT_ASPECT_RATIO = 4.0f / 3.0f;

constexpr float MIN_FOV = 1.0f;
constexpr float MAX_FOV = 135.0f;

class Camera : public UniformSource, public ViewSource {
 public:
  // Constructs a new Camera. Angular values should be provided in degrees.
  Camera(glm::vec3 position = glm::vec3(0.0f),
         glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
         float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH,
         float fov = DEFAULT_FOV, float aspectRatio = DEFAULT_ASPECT_RATIO,
         float near = DEFAULT_NEAR, float far = DEFAULT_FAR);
  virtual ~Camera() = default;

  void lookAt(glm::vec3 center);

  glm::vec3 getPosition() const { return position_; }
  void setPosition(glm::vec3 position) { position_ = position; }
  float getYaw() const { return yaw_; }
  void setYaw(float yaw) {
    yaw_ = yaw;
    updateCameraVectors();
  }
  float getPitch() const { return pitch_; }
  void setPitch(float pitch) {
    pitch_ = pitch;
    updateCameraVectors();
  }
  float getFov() const { return fov_; }
  void setFov(float fov) { fov_ = fov; }

  float getAspectRatio() const { return aspectRatio_; }
  void setAspectRatio(float aspectRatio) { aspectRatio_ = aspectRatio; }
  void setAspectRatio(ImageSize size) {
    aspectRatio_ = size.width / static_cast<float>(size.height);
  }

  glm::mat4 getViewTransform() const override;
  glm::mat4 getProjectionTransform() const;

  void updateUniforms(Shader& shader) override;

  // Moves the camera in the given direction by the given amount.
  void move(CameraDirection direction, float velocity);
  // Rotates the camera by the given yaw and pitch offsets. If constrainPitch is
  // true, pitch is clamped when near the up and down directions.
  void rotate(float yawOffset, float pitchOffset, bool constrainPitch = true);
  // Changes the zoom level of the FoV by the given offset. Alternative to
  // setFov().
  void zoom(float fovOffset);

 private:
  void updateCameraVectors();

  glm::vec3 position_;
  glm::vec3 front_;
  glm::vec3 up_;
  glm::vec3 right_;
  glm::vec3 worldUp_;

  // The **clockwise** angle that the camera is facing, measured from the
  // positive X axis.
  float yaw_;
  float pitch_;
  float fov_;

  float aspectRatio_;
  float near_;
  float far_;
};

struct MouseDelta {
  float xoffset;
  float yoffset;
};

// Interface for camera controllers.
class CameraControls {
 public:
  virtual void resizeWindow(int width, int height) = 0;
  virtual void scroll(Camera& camera, double xoffset, double yoffset,
                      bool mouseCaptured) = 0;
  virtual void mouseMove(Camera& camera, double xpos, double ypos,
                         bool mouseCaptured) = 0;
  virtual void mouseButton(Camera& camera, int button, int action, int mods,
                           bool mouseCaptured) = 0;
  virtual void processInput(GLFWwindow* window, Camera& camera,
                            float deltaTime) = 0;

  // Base helpers; call these from subclasses.
  void handleDragStartEnd(int button, int action);
  MouseDelta calculateMouseDelta(double xpos, double ypos);

  float getSpeed() const { return speed_; }
  void setSpeed(float speed) { speed_ = speed; }
  float getSensitivity() const { return sensitivity_; }
  void setSensitivity(float sensitivity) { sensitivity_ = sensitivity; }

 protected:
  static constexpr float DEFAULT_SPEED = 5.0f;
  static constexpr float DEFAULT_SENSITIVITY = 0.1f;

  // Whether input updates should use the last mouse positions to calculate
  // delta, or to start from the current positions.
  bool initialized_ = false;
  // Whether a mouse is being click+dragged.
  bool dragging_ = false;
  float lastMouseX_;
  float lastMouseY_;

  // General setting for camera movement speed.
  float speed_ = DEFAULT_SPEED;
  // General setting for camera sensitivity.
  float sensitivity_ = DEFAULT_SENSITIVITY;
};

// Camera controls that implement a fly mode, similar to DCC tools.
class FlyCameraControls : public CameraControls {
 public:
  virtual ~FlyCameraControls() = default;
  void resizeWindow(int width, int height) override;
  void scroll(Camera& camera, double xoffset, double yoffset,
              bool mouseCaptured) override;
  void mouseMove(Camera& camera, double xpos, double ypos,
                 bool mouseCaptured) override;
  void mouseButton(Camera& camera, int button, int action, int mods,
                   bool mouseCaptured) override;
  void processInput(GLFWwindow* window, Camera& camera,
                    float deltaTime) override;
};

// Camera controls that implement an orbit mode at a focal point.
class OrbitCameraControls : public CameraControls {
 public:
  OrbitCameraControls(Camera& camera, glm::vec3 center = glm::vec3(0.0f));
  virtual ~OrbitCameraControls() = default;
  void resizeWindow(int width, int height) override;
  void scroll(Camera& camera, double xoffset, double yoffset,
              bool mouseCaptured) override;
  void mouseMove(Camera& camera, double xpos, double ypos,
                 bool mouseCaptured) override;
  void mouseButton(Camera& camera, int button, int action, int mods,
                   bool mouseCaptured) override;
  void processInput(GLFWwindow* window, Camera& camera,
                    float deltaTime) override;

  glm::vec3 getCenter() const { return center_; }
  void setCenter(glm::vec3 center) { center_ = center; }

  // Updates the camera position and orientation to match the current orbit
  // position.
  void updateCamera(Camera& camera);

 private:
  static constexpr float MIN_RADIUS = 0.1f;
  static constexpr float MAX_RADIUS = 100.0f;
  // The center point that we're orbiting around.
  glm::vec3 center_;
  // The distance from the center.
  float radius_;
  // The **clockwise** angle in degrees that the camera is rotated about the
  // center.
  float azimuth_;
  // The angle in degrees that the camera is tilted up or down.
  float altitude_;
};

}  // namespace qrk

#endif
