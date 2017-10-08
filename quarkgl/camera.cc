#include <qrk/camera.h>

namespace qrk {

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch,
               float speed, float sensitivity, float fov)
    : position_(position),
      worldUp_(worldUp),
      yaw_(yaw),
      pitch_(pitch),
      speed_(speed),
      sensitivity_(sensitivity),
      fov_(fov) {
  updateCameraVectors();
}

void Camera::updateCameraVectors() {
  glm::vec3 front;
  front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front.y = sin(glm::radians(pitch_));
  front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));

  front_ = glm::normalize(front);
  right_ = glm::normalize(glm::cross(front_, worldUp_));
  up_ = glm::normalize(glm::cross(right_, front_));
}

glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position_, position_ + front_, up_);
}

void Camera::processKeyboard(CameraDirection direction, float deltaTime) {
  float velocity = speed_ * deltaTime;
  if (direction == CameraDirection::FORWARD) {
    position_ += front_ * velocity;
  }
  if (direction == CameraDirection::BACKWARD) {
    position_ -= front_ * velocity;
  }
  if (direction == CameraDirection::LEFT) {
    position_ -= right_ * velocity;
  }
  if (direction == CameraDirection::RIGHT) {
    position_ += right_ * velocity;
  }
}

void Camera::processMouseMove(float xoffset, float yoffset,
                              bool constrainPitch) {
  xoffset *= sensitivity_;
  yoffset *= sensitivity_;

  // Constrain yaw to be 0-360 to avoid floating point error.
  yaw_ = glm::mod(yaw_ + xoffset, 360.0f);
  pitch_ += yoffset;

  if (constrainPitch) {
    pitch_ = glm::clamp(pitch_, -89.0f, 89.0f);
  }

  updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset) {
  fov_ = glm::clamp(fov_ - yoffset, MIN_FOV, MAX_FOV);
}
}  // namespace qrk
