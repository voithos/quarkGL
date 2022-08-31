#include <qrk/shadows.h>

namespace qrk {
namespace {
constexpr float SHADOW_CAMERA_DISTANCE = 7.0f;
// TODO: Parameterize these shadow numbers.
constexpr float NEAR_PLANE = 0.1f;
constexpr float FAR_PLANE = 15.0f;
}  // namespace

ShadowCamera::ShadowCamera(std::shared_ptr<DirectionalLight> light,
                           float cuboidExtents, glm::vec3 worldUp)
    : light_(light), cuboidExtents_(cuboidExtents), worldUp_(worldUp) {}

glm::mat4 ShadowCamera::getViewTransform() {
  return glm::lookAt(SHADOW_CAMERA_DISTANCE * -light_->getDirection(),
                     glm::vec3(0.0f), worldUp_);
}

// TODO: Rename to getProjectionTransform.
glm::mat4 ShadowCamera::getPerspectiveTransform() {
  // Directional lights cast orthographic shadows.
  return glm::ortho(-cuboidExtents_, cuboidExtents_, -cuboidExtents_,
                    cuboidExtents_, NEAR_PLANE, FAR_PLANE);
}

void ShadowCamera::updateUniforms(Shader& shader) {
  shader.setMat4("lightViewProjection",
                 getPerspectiveTransform() * getViewTransform());
}

}  // namespace qrk
