#include <qrk/shadows.h>

namespace qrk {

ShadowCamera::ShadowCamera(std::shared_ptr<DirectionalLight> light,

                           float cuboidExtents, float near, float far,
                           float shadowCameraDistanceFromOrigin,
                           glm::vec3 worldUp)
    : light_(light),
      cuboidExtents_(cuboidExtents),
      near_(near),
      far_(far),
      shadowCameraDistanceFromOrigin_(shadowCameraDistanceFromOrigin),
      worldUp_(worldUp) {}

glm::mat4 ShadowCamera::getViewTransform() {
  return glm::lookAt(shadowCameraDistanceFromOrigin_ * -light_->getDirection(),
                     glm::vec3(0.0f), worldUp_);
}

glm::mat4 ShadowCamera::getProjectionTransform() {
  // Directional lights cast orthographic shadows.
  return glm::ortho(-cuboidExtents_, cuboidExtents_, -cuboidExtents_,
                    cuboidExtents_, near_, far_);
}

void ShadowCamera::updateUniforms(Shader& shader) {
  shader.setMat4("lightViewProjection",
                 getProjectionTransform() * getViewTransform());
}

}  // namespace qrk
