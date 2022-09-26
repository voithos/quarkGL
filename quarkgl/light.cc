#include <qrk/light.h>

namespace qrk {
void LightRegistry::addLight(std::shared_ptr<Light> light) {
  // TODO: Throw an error if this exceeds the max light count supported in the
  // shader.
  lights_.push_back(light);

  switch (light->getLightType()) {
    case LightType::DIRECTIONAL_LIGHT:
      light->setLightIdx(directionalCount_);
      directionalCount_++;
      break;
    case LightType::POINT_LIGHT:
      light->setLightIdx(pointCount_);
      pointCount_++;
      break;
    case LightType::SPOT_LIGHT:
      light->setLightIdx(spotCount_);
      spotCount_++;
      break;
  }
}

void LightRegistry::updateUniforms(Shader& shader) {
  if (viewSource_ != nullptr) {
    applyViewTransform(viewSource_->getViewTransform());
  }
  shader.setInt("qrk_directionalLightCount", directionalCount_);
  shader.setInt("qrk_pointLightCount", pointCount_);
  shader.setInt("qrk_spotLightCount", spotCount_);

  for (auto light : lights_) {
    light->updateUniforms(shader);
  }
}

void LightRegistry::applyViewTransform(const glm::mat4& view) {
  // TODO: Only do this when we need to.
  for (auto light : lights_) {
    light->applyViewTransform(view);
  }
}

void LightRegistry::setUseViewTransform(bool useViewTransform) {
  for (auto light : lights_) {
    light->setUseViewTransform(useViewTransform);
  }
}

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 ambient,
                                   glm::vec3 diffuse, glm::vec3 specular)
    : direction_(glm::normalize(direction)),
      ambient_(ambient),
      diffuse_(diffuse),
      specular_(specular) {}

void DirectionalLight::updateUniforms(Shader& shader) {
  checkState();

  if (hasViewBeenApplied_) {
    shader.setVec3(uniformName_ + ".direction",
                   useViewTransform_ ? viewDirection_ : direction_);
  }
  if (hasLightChanged_) {
    shader.setVec3(uniformName_ + ".ambient", ambient_);
    shader.setVec3(uniformName_ + ".diffuse", diffuse_);
    shader.setVec3(uniformName_ + ".specular", specular_);
  }

  // TODO: Fix change detection to work with >1 shaders.
  // resetChangeDetection();
}

void DirectionalLight::applyViewTransform(const glm::mat4& view) {
  // TODO: This should probably be inverse+transpose of the view.
  viewDirection_ = glm::vec3(view * glm::vec4(direction_, 0.0f));
  hasViewBeenApplied_ = true;
}

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse,
                       glm::vec3 specular, Attenuation attenuation)
    : position_(position),
      ambient_(ambient),
      diffuse_(diffuse),
      specular_(specular),
      attenuation_(attenuation) {}

void PointLight::updateUniforms(Shader& shader) {
  checkState();

  if (hasViewBeenApplied_) {
    shader.setVec3(uniformName_ + ".position",
                   useViewTransform_ ? viewPosition_ : position_);
  }
  if (hasLightChanged_) {
    shader.setVec3(uniformName_ + ".ambient", ambient_);
    shader.setVec3(uniformName_ + ".diffuse", diffuse_);
    shader.setVec3(uniformName_ + ".specular", specular_);
    shader.setFloat(uniformName_ + ".attenuation.constant",
                    attenuation_.constant);
    shader.setFloat(uniformName_ + ".attenuation.linear", attenuation_.linear);
    shader.setFloat(uniformName_ + ".attenuation.quadratic",
                    attenuation_.quadratic);
  }

  // resetChangeDetection();
}

void PointLight::applyViewTransform(const glm::mat4& view) {
  viewPosition_ = glm::vec3(view * glm::vec4(position_, 1.0f));
  hasViewBeenApplied_ = true;
}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, float innerAngle,
                     float outerAngle, glm::vec3 ambient, glm::vec3 diffuse,
                     glm::vec3 specular, Attenuation attenuation)
    : position_(position),
      direction_(direction),
      innerAngle_(innerAngle),
      outerAngle_(outerAngle),
      ambient_(ambient),
      diffuse_(diffuse),
      specular_(specular),
      attenuation_(attenuation) {}

void SpotLight::updateUniforms(Shader& shader) {
  checkState();

  if (hasViewBeenApplied_) {
    shader.setVec3(uniformName_ + ".position",
                   useViewTransform_ ? viewPosition_ : position_);
    shader.setVec3(uniformName_ + ".direction",
                   useViewTransform_ ? viewDirection_ : direction_);
  }
  if (hasLightChanged_) {
    shader.setFloat(uniformName_ + ".innerAngle", innerAngle_);
    shader.setFloat(uniformName_ + ".outerAngle", outerAngle_);
    shader.setVec3(uniformName_ + ".ambient", ambient_);
    shader.setVec3(uniformName_ + ".diffuse", diffuse_);
    shader.setVec3(uniformName_ + ".specular", specular_);
    shader.setFloat(uniformName_ + ".attenuation.constant",
                    attenuation_.constant);
    shader.setFloat(uniformName_ + ".attenuation.linear", attenuation_.linear);
    shader.setFloat(uniformName_ + ".attenuation.quadratic",
                    attenuation_.quadratic);
  }

  // resetChangeDetection();
}

void SpotLight::applyViewTransform(const glm::mat4& view) {
  viewPosition_ = glm::vec3(view * glm::vec4(position_, 1.0f));
  // TODO: This should probably be inverse+transpose of the view.
  viewDirection_ = glm::vec3(view * glm::vec4(direction_, 0.0f));
  hasViewBeenApplied_ = true;
}

}  // namespace qrk
