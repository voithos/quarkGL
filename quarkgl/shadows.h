#ifndef QUARKGL_SHADOWS_H_
#define QUARKGL_SHADOWS_H_

#include <qrk/exceptions.h>
#include <qrk/framebuffer.h>
#include <qrk/light.h>
#include <qrk/shader.h>
#include <qrk/texture.h>
#include <qrk/texture_registry.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace qrk {

class ShadowException : public QuarkException {
  using QuarkException::QuarkException;
};

class ShadowCamera : public UniformSource {
 public:
  // TODO: Currently only renders the origin. Make this more dynamic, and have
  // it automatically determine a best-fit frustum based on the scene.
  ShadowCamera(std::shared_ptr<DirectionalLight> light,
               float cuboidExtents = 10.0f, float near = 0.1f,
               float far = 15.0f, float shadowCameraDistanceFromOrigin = 7.0f,
               glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));
  virtual ~ShadowCamera() = default;

  float getCuboidExtents() const { return cuboidExtents_; }
  void setCuboidExtents(float cuboidExtents) { cuboidExtents_ = cuboidExtents; }
  float getNearPlane() const { return near_; }
  void setNearPlane(float near) { near_ = near; }
  float getFarPlane() const { return far_; }
  void setFarPlane(float far) { far_ = far; }
  float getDistanceFromOrigin() const {
    return shadowCameraDistanceFromOrigin_;
  }
  void setDistanceFromOrigin(float dist) {
    shadowCameraDistanceFromOrigin_ = dist;
  }

  glm::mat4 getViewTransform();
  glm::mat4 getProjectionTransform();

  void updateUniforms(Shader& shader) override;

 private:
  std::shared_ptr<DirectionalLight> light_;
  // The extents of the ortho camera's rendering cuboid.
  float cuboidExtents_;
  // The near plane of the ortho cuboid.
  float near_;
  // The far plane of the ortho cuboid.
  float far_;
  // The fake distance from the origin that the shadow camera is positioned at.
  float shadowCameraDistanceFromOrigin_;
  glm::vec3 worldUp_;
};

class ShadowMap : public Framebuffer, public TextureSource {
 public:
  explicit ShadowMap(int width = 1024, int height = 1024);
  explicit ShadowMap(ImageSize size) : ShadowMap(size.width, size.height) {}
  virtual ~ShadowMap() = default;

  Texture getDepthTexture() { return depthAttachment_.asTexture(); }
  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Attachment depthAttachment_;
};

}  // namespace qrk

#endif
