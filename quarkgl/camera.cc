#include <qrk/camera.h>

namespace qrk {

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch,
               float speed, float sensitivity, float fov, float aspectRatio,
               float near, float far)
    : position_(position),
      worldUp_(worldUp),
      yaw_(yaw),
      pitch_(pitch),
      speed_(speed),
      sensitivity_(sensitivity),
      fov_(fov),
      aspectRatio_(aspectRatio),
      near_(near),
      far_(far) {
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

glm::mat4 Camera::getViewTransform() {
  return glm::lookAt(position_, position_ + front_, up_);
}

glm::mat4 Camera::getPerspectiveTransform() {
  return glm::perspective(glm::radians(getFov()), aspectRatio_, near_, far_);
}

void Camera::move(CameraDirection direction, float deltaTime) {
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

void Camera::rotate(float xoffset, float yoffset, bool constrainPitch) {
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

void Camera::zoom(float offset) {
  fov_ = glm::clamp(fov_ - offset, MIN_FOV, MAX_FOV);
}
}  // namespace qrk
