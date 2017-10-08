#ifndef QUARKGL_CAMERA_H_
#define QUARKGL_CAMERA_H_

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace qrk {

enum class CameraDirection {
  FORWARD,
  BACKWARD,
  LEFT,
  RIGHT,
};

const float DEFAULT_YAW = 270.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_SPEED = 2.5f;
const float DEFAULT_SENSITIVITY = 0.1f;
const float DEFAULT_FOV = 45.0f;

const float MIN_FOV = 1.0f;
const float MAX_FOV = 45.0f;

class Camera {
 private:
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

  void updateCameraVectors();

 public:
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f),
         float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH,
         float speed = DEFAULT_SPEED, float sensitivity = DEFAULT_SENSITIVITY,
         float fov = DEFAULT_FOV);

  glm::vec3 getPosition() { return position_; }
  glm::vec3 getFront() { return front_; }
  float getFov() { return fov_; }

  glm::mat4 getViewMatrix();
  void processKeyboard(CameraDirection direction, float deltaTime);
  void processMouseMove(float xoffset, float yoffset,
                        bool constrainPitch = true);
  void processMouseScroll(float yoffset);
};
}  // namespace qrk

#endif
