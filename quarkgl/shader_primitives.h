#ifndef QUARKGL_SHADER_PRIMITIVES_H_
#define QUARKGL_SHADER_PRIMITIVES_H_

#include <qrk/shader.h>

#include <glm/glm.hpp>

namespace qrk {

// A builtin shader for skyboxes. Should be used at the end of a frame to render
// the skybox behind all other geometry.
class SkyboxShader : public Shader {
 public:
  SkyboxShader();

  virtual void activate() override;
  virtual void deactivate() override;

  virtual void setMat4(const char* name, const glm::mat4& matrix) override;
};

class ScreenQuadShader : public Shader {
 public:
  ScreenQuadShader();
};

}  // namespace qrk

#endif