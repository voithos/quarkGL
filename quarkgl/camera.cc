#include <qrk/camera.h>

#include <iostream>

namespace qrk {

namespace {
constexpr float POLAR_CAP = 90.0f - 0.1f;
}

Camera::Camera(glm::vec3 position, glm::vec3 worldUp, float yaw, float pitch,
               float fov, float aspectRatio, float near, float far)
    : position_(position),
      worldUp_(worldUp),
      yaw_(yaw),
      pitch_(pitch),
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

void Camera::move(CameraDirection direction, float velocity) {
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

void Camera::rotate(float yawOffset, float pitchOffset, bool constrainPitch) {
  // Constrain yaw to be 0-360 to avoid floating point error.
  yaw_ = glm::mod(yaw_ + yawOffset, 360.0f);
  pitch_ += pitchOffset;

  if (constrainPitch) {
    pitch_ = glm::clamp(pitch_, -POLAR_CAP, POLAR_CAP);
  }

  updateCameraVectors();
}

void Camera::zoom(float offset) {
  fov_ = glm::clamp(fov_ - offset, MIN_FOV, MAX_FOV);
}

void CameraControls::handleDragStartEnd(int button, int action) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    dragging_ = true;
    initialized_ = false;
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    dragging_ = false;
  }
}

MouseDelta CameraControls::calculateMouseDelta(double xpos, double ypos) {
  if (!initialized_) {
    lastMouseX_ = xpos;
    lastMouseY_ = ypos;
    initialized_ = true;
  }
  float xoffset = xpos - lastMouseX_;
  // Reversed since y-coordinates range from bottom to top.
  float yoffset = lastMouseY_ - ypos;
  lastMouseX_ = xpos;
  lastMouseY_ = ypos;
  return MouseDelta{xoffset, yoffset};
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

  MouseDelta delta = calculateMouseDelta(xpos, ypos);

  camera.rotate(delta.xoffset * sensitivity_, delta.yoffset * sensitivity_);
}

void FlyCameraControls::mouseButton(Camera& camera, int button, int action,
                                    int mods, bool mouseCaptured) {
  handleDragStartEnd(button, action);
}

void FlyCameraControls::processInput(GLFWwindow* window, Camera& camera,
                                     float deltaTime) {
  float velocity = speed_ * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::FORWARD, velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::LEFT, velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::BACKWARD, velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::RIGHT, velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::UP, velocity);
  }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    camera.move(qrk::CameraDirection::DOWN, velocity);
  }
}

OrbitCameraControls::OrbitCameraControls(Camera& camera, glm::vec3 center)
    : center_(center) {
  glm::vec3 dir = camera.getPosition() - center;
  radius_ = glm::clamp(glm::length(dir), MIN_RADIUS, MAX_RADIUS);
  glm::vec3 normdir = glm::normalize(dir);
  // TODO: Fix this, atan2 args are backwards.
  azimuth_ = glm::mod<float>(glm::degrees(atan2(normdir.x, normdir.z)) * -1.0f,
                             360.0f) +
             90.0f;
  altitude_ =
      glm::clamp<float>(glm::degrees(asin(normdir.y)), -POLAR_CAP, POLAR_CAP);

  updateCamera(camera);
}

void OrbitCameraControls::resizeWindow(int width, int height) {}

void OrbitCameraControls::scroll(Camera& camera, double xoffset, double yoffset,
                                 bool mouseCaptured) {
  radius_ =
      glm::clamp(radius_ - static_cast<float>(yoffset), MIN_RADIUS, MAX_RADIUS);
  updateCamera(camera);
}

void OrbitCameraControls::mouseMove(Camera& camera, double xpos, double ypos,
                                    bool mouseCaptured) {
  // Only move when dragging, or when the mouse is captured.
  if (!(dragging_ || mouseCaptured)) {
    return;
  }

  MouseDelta delta = calculateMouseDelta(xpos, ypos);

  // Constrain azimuth to be 0-360 to avoid floating point error.
  azimuth_ = glm::mod(azimuth_ + delta.xoffset * sensitivity_, 360.0f);
  altitude_ = glm::clamp(altitude_ - delta.yoffset * sensitivity_, -POLAR_CAP,
                         POLAR_CAP);

  updateCamera(camera);
}

void OrbitCameraControls::mouseButton(Camera& camera, int button, int action,
                                      int mods, bool mouseCaptured) {
  handleDragStartEnd(button, action);
}

void OrbitCameraControls::processInput(GLFWwindow* window, Camera& camera,
                                       float deltaTime) {}

void OrbitCameraControls::updateCamera(Camera& camera) {
  // Compute camera position.
  glm::vec3 cameraPosition;
  cameraPosition.x = center_.x + radius_ * cos(glm::radians(altitude_)) *
                                     cos(glm::radians(azimuth_));
  cameraPosition.y = center_.y + radius_ * sin(glm::radians(altitude_));
  cameraPosition.z = center_.z + radius_ * cos(glm::radians(altitude_)) *
                                     sin(glm::radians(azimuth_));

  camera.setPosition(cameraPosition);
  camera.lookAt(center_);
}

}  // namespace qrk
