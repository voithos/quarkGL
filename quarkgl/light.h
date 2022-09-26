#ifndef QUARKGL_LIGHT_H_
#define QUARKGL_LIGHT_H_

#include <qrk/exceptions.h>
#include <qrk/shader.h>

#include <glm/glm.hpp>
#include <string>
#include <vector>

namespace qrk {

class LightException : public QuarkException {
  using QuarkException::QuarkException;
};

struct Attenuation {
  float constant;
  float linear;
  float quadratic;
};

constexpr Attenuation DEFAULT_ATTENUATION = {1.0f, 0.09f, 0.032f};
constexpr glm::vec3 DEFAULT_AMBIENT = glm::vec3(0.1f, 0.1f, 0.1f);
constexpr glm::vec3 DEFAULT_DIFFUSE = glm::vec3(0.5f, 0.5f, 0.5f);
constexpr glm::vec3 DEFAULT_SPECULAR = glm::vec3(1.0f, 1.0f, 1.0f);

constexpr float DEFAULT_INNER_ANGLE = glm::radians(10.5f);
constexpr float DEFAULT_OUTER_ANGLE = glm::radians(19.5f);

enum class LightType {
  DIRECTIONAL_LIGHT,
  POINT_LIGHT,
  SPOT_LIGHT,
};

class LightRegistry;

class Light {
 public:
  virtual ~Light() = default;
  virtual LightType getLightType() const = 0;

  void setUseViewTransform(bool useViewTransform) {
    useViewTransform_ = useViewTransform;
  }

  friend LightRegistry;

 protected:
  void setLightIdx(unsigned int lightIdx) {
    lightIdx_ = lightIdx;
    uniformName_ = getUniformName(lightIdx);
  }

  void checkState() {
    if (hasViewDependentChanged_ && !hasViewBeenApplied_) {
      throw LightException(
          "ERROR::LIGHT::VIEW_CHANGED\n"
          "Light state changed without re-applying view transform.");
    }
  }

  void resetChangeDetection() {
    hasViewDependentChanged_ = false;
    hasLightChanged_ = false;
    hasViewBeenApplied_ = false;
  }

  virtual std::string getUniformName(unsigned int lightIdx) const = 0;
  virtual void updateUniforms(Shader& shader) = 0;
  virtual void applyViewTransform(const glm::mat4& view) = 0;

  unsigned int lightIdx_;
  std::string uniformName_;

  // Whether the light's position uniforms should be in view space. If false,
  // the positions are instead in world space.
  bool useViewTransform_ = true;
  // Start as `true` so that initial uniform values get set.
  bool hasViewDependentChanged_ = true;
  bool hasLightChanged_ = true;

  bool hasViewBeenApplied_ = false;
};

// A source for the camera view transform.
class ViewSource {
 public:
  virtual glm::mat4 getViewTransform() const = 0;
};

class LightRegistry : public UniformSource {
 public:
  virtual ~LightRegistry() = default;
  void addLight(std::shared_ptr<Light> light);
  // Sets the view source used to update the light uniforms. The source is
  // called when updating uniforms.
  void setViewSource(std::shared_ptr<ViewSource> viewSource) {
    viewSource_ = viewSource;
  }
  void updateUniforms(Shader& shader);

  // Applies the view transform to the registered lights. This is automatically
  // called if a view source has been set.
  void applyViewTransform(const glm::mat4& view);

  // Sets whether the registered lights should transform their positions to view
  // space. If false, positions remain in world space when passed to the shader.
  void setUseViewTransform(bool useViewTransform);

 private:
  unsigned int directionalCount_ = 0;
  unsigned int pointCount_ = 0;
  unsigned int spotCount_ = 0;

  std::shared_ptr<ViewSource> viewSource_;
  std::vector<std::shared_ptr<Light>> lights_;
};

class DirectionalLight : public Light {
 public:
  DirectionalLight(glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f),
                   glm::vec3 ambient = DEFAULT_AMBIENT,
                   glm::vec3 diffuse = DEFAULT_DIFFUSE,
                   glm::vec3 specular = DEFAULT_SPECULAR);

  LightType getLightType() const { return LightType::DIRECTIONAL_LIGHT; }

  glm::vec3 getDirection() const { return direction_; }
  void setDirection(glm::vec3 direction) {
    direction_ = direction;
    hasViewDependentChanged_ = true;
  }
  glm::vec3 getAmbient() const { return ambient_; }
  void setAmbient(glm::vec3 ambient) {
    ambient_ = ambient;
    hasLightChanged_ = true;
  }
  glm::vec3 getDiffuse() const { return diffuse_; }
  void setDiffuse(glm::vec3 diffuse) {
    diffuse_ = diffuse;
    hasLightChanged_ = true;
  }
  glm::vec3 getSpecular() const { return specular_; }
  void setSpecular(glm::vec3 specular) {
    specular_ = specular;
    hasLightChanged_ = true;
  }

 protected:
  std::string getUniformName(unsigned int lightIdx) const {
    return "qrk_directionalLights[" + std::to_string(lightIdx) + "]";
  }
  void updateUniforms(Shader& shader);
  void applyViewTransform(const glm::mat4& view);

 private:
  glm::vec3 direction_;
  glm::vec3 viewDirection_;

  glm::vec3 ambient_;
  glm::vec3 diffuse_;
  glm::vec3 specular_;
};

class PointLight : public Light {
 public:
  PointLight(glm::vec3 position = glm::vec3(0.0f),
             glm::vec3 ambient = DEFAULT_AMBIENT,
             glm::vec3 diffuse = DEFAULT_DIFFUSE,
             glm::vec3 specular = DEFAULT_SPECULAR,
             Attenuation attenuation = DEFAULT_ATTENUATION);

  LightType getLightType() const { return LightType::POINT_LIGHT; }

  glm::vec3 getPosition() const { return position_; }
  void setPosition(glm::vec3 position) {
    position_ = position;
    hasViewDependentChanged_ = true;
  }
  glm::vec3 getAmbient() const { return ambient_; }
  void setAmbient(glm::vec3 ambient) {
    ambient_ = ambient;
    hasLightChanged_ = true;
  }
  glm::vec3 getDiffuse() const { return diffuse_; }
  void setDiffuse(glm::vec3 diffuse) {
    diffuse_ = diffuse;
    hasLightChanged_ = true;
  }
  glm::vec3 getSpecular() const { return specular_; }
  void setSpecular(glm::vec3 specular) {
    specular_ = specular;
    hasLightChanged_ = true;
  }
  Attenuation getAttenuation() const { return attenuation_; }
  void setAttenuation(Attenuation attenuation) {
    attenuation_ = attenuation;
    hasLightChanged_ = true;
  }

 protected:
  std::string getUniformName(unsigned int lightIdx) const {
    return "qrk_pointLights[" + std::to_string(lightIdx) + "]";
  }
  void updateUniforms(Shader& shader);
  void applyViewTransform(const glm::mat4& view);

 private:
  glm::vec3 position_;
  glm::vec3 viewPosition_;

  glm::vec3 ambient_;
  glm::vec3 diffuse_;
  glm::vec3 specular_;

  Attenuation attenuation_;
};

class SpotLight : public Light {
 public:
  SpotLight(glm::vec3 position = glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3 direction = glm::vec3(0.0f, -1.0f, 0.0f),
            float innerAngle = DEFAULT_INNER_ANGLE,
            float outerAngle = DEFAULT_OUTER_ANGLE,
            glm::vec3 ambient = DEFAULT_AMBIENT,
            glm::vec3 diffuse = DEFAULT_DIFFUSE,
            glm::vec3 specular = DEFAULT_SPECULAR,
            Attenuation attenuation = DEFAULT_ATTENUATION);

  LightType getLightType() const { return LightType::SPOT_LIGHT; }

  glm::vec3 getPosition() const { return position_; }
  void setPosition(glm::vec3 position) {
    position_ = position;
    hasViewDependentChanged_ = true;
  }
  glm::vec3 getDirection() const { return direction_; }
  void setDirection(glm::vec3 direction) {
    direction_ = direction;
    hasViewDependentChanged_ = true;
  }
  glm::vec3 getAmbient() const { return ambient_; }
  void setAmbient(glm::vec3 ambient) {
    ambient_ = ambient;
    hasLightChanged_ = true;
  }
  glm::vec3 getDiffuse() const { return diffuse_; }
  void setDiffuse(glm::vec3 diffuse) {
    diffuse_ = diffuse;
    hasLightChanged_ = true;
  }
  glm::vec3 getSpecular() const { return specular_; }
  void setSpecular(glm::vec3 specular) {
    specular_ = specular;
    hasLightChanged_ = true;
  }
  Attenuation getAttenuation() const { return attenuation_; }
  void setAttenuation(Attenuation attenuation) {
    attenuation_ = attenuation;
    hasLightChanged_ = true;
  }

 protected:
  std::string getUniformName(unsigned int lightIdx) const {
    return "qrk_spotLights[" + std::to_string(lightIdx) + "]";
  }
  void updateUniforms(Shader& shader);
  void applyViewTransform(const glm::mat4& view);

 private:
  glm::vec3 position_;
  glm::vec3 viewPosition_;
  glm::vec3 direction_;
  glm::vec3 viewDirection_;

  float innerAngle_;
  float outerAngle_;

  glm::vec3 ambient_;
  glm::vec3 diffuse_;
  glm::vec3 specular_;

  Attenuation attenuation_;
};

}  // namespace qrk

#endif
