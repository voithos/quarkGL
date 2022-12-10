#include <qrk/camera.h>

#include <iostream>

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

void Camera::lookAt(glm::vec3 center) {
  glm::vec3 dir = glm::normalize(center - position_);
  pitch_ = glm::degrees(asin(dir.y));
  yaw_ = glm::mod<float>(glm::degrees(atan2(dir.x, dir.z)) * -1.0f, 360.0f) +
         90.0f;
  updateCameraVectors();
}

glm::mat4 Camera::getViewTransform() const {
  glm::vec3 center = position_ + front_;
  return glm::lookAt(/*eye=*/position_, center, up_);
}

glm::mat4 Camera::getProjectionTransform() const {
  return glm::perspective(glm::radians(getFov()), aspectRatio_, near_, far_);
}

void Camera::updateUniforms(Shader& shader) {
  shader.setMat4("view", getViewTransform());
  shader.setMat4("projection", getProjectionTransform());
}

void Camera::move(CameraDirection direction, float deltaTime) {
  float velocity = speed_ * deltaTime;
  switch (direction) {
    case CameraDirection::FORWARD:
      position_ += front_ * velocity;
      break;
    case CameraDirection::BACKWARD:
      position_ -= front_ * velocity;
      break;
    case CameraDirection::LEFT:
      position_ -= right_ * velocity;
      break;
    case CameraDirection::RIGHT:
      position_ += right_ * velocity;
      break;
    case CameraDirection::UP:
      position_ += up_ * velocity;
      break;
    case CameraDirection::DOWN:
      position_ -= up_ * velocity;
      break;
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

void FlyCameraControls::resizeWindow(int width, int height) {}

void FlyCameraControls::scroll(Camera& camera, double xoffset, double yoffset,
                               bool mouseCaptured) {
  // Always respond to scroll.
  camera.zoom(yoffset);
}

void FlyCameraControls::mouseMove(Camera& camera, double xpos, double ypos,
                                  bool mouseCaptured) {
  // Only move when dragging, or when the mouse is captured.
  if (!(dragging_ || mouseCaptured)) {
    return;
  }

  if (!initialized_) {
    lastX_ = xpos;
    lastY_ = ypos;
    initialized_ = true;
  }
  float xoffset = xpos - lastX_;
  // Reversed since y-coordinates range from bottom to top.
  float yoffset = lastY_ - ypos;
  lastX_ = xpos;
  lastY_ = ypos;

  camera.rotate(xoffset, yoffset);
}

void FlyCameraControls::mouseButton(Camera& camera, int button, int action,
                                    int mods, bool mouseCaptured) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    dragging_ = true;
    initialized_ = false;
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    dragging_ = false;
  }
}

void FlyCameraControls::processInput(GLFWwindow* window, Camera& camera,
                                     float deltaTime) {
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::FORWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::LEFT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::BACKWARD, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::RIGHT, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::UP, deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::DOWN, deltaTime);
  }
}

}  // namespace qrk
