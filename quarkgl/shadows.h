#ifndef QUARKGL_SHADOWS_H_
#define QUARKGL_SHADOWS_H_

#include <qrk/exceptions.h>
#include <qrk/light.h>
#include <qrk/shader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace qrk {

class ShadowException : public QuarkException {
  using QuarkException::QuarkException;
};

class ShadowCamera : public UniformSource {
 public:
  // TODO: Currently only renders the origin.
  explicit ShadowCamera(std::shared_ptr<DirectionalLight> light,
                        float cuboidExtents = 10.0f,
                        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f));
  virtual ~ShadowCamera() = default;

  glm::mat4 getViewTransform();
  glm::mat4 getPerspectiveTransform();

  void updateUniforms(Shader& shader) override;

 private:
  std::shared_ptr<DirectionalLight> light_;
  // The extents  of the ortho camera's rendering cuboid.
  float cuboidExtents_;
  glm::vec3 worldUp_;
};

}  // namespace qrk

#endif
