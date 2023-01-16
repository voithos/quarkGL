#ifndef QUARKGL_IBL_H_
#define QUARKGL_IBL_H_

#include <qrk/cubemap.h>
#include <qrk/framebuffer.h>
#include <qrk/mesh_primitives.h>
#include <qrk/shader.h>
#include <qrk/shader_primitives.h>
#include <qrk/texture_registry.h>

namespace qrk {

class IblException : public QuarkException {
  using QuarkException::QuarkException;
};

class CubemapIrradianceShader : public Shader {
 public:
  CubemapIrradianceShader();

  void setHemisphereSampleDelta(float delta);
};

// Calculates a diffuse irradiance map based on a given HDR cubemap.
// Note that since each pixel in the irradiance map is uniformly sampled from an
// entire hemisphere, fine details are not preserved - thus the irradiance map
// need not be very large (32x32 can suffice), and the texture's bilinear
// filtering can fill in the blanks.
class CubemapIrradianceCalculator : public TextureSource {
 public:
  CubemapIrradianceCalculator(int width = 32, int height = 32);
  explicit CubemapIrradianceCalculator(ImageSize size)
      : CubemapIrradianceCalculator(size.width, size.height) {}
  virtual ~CubemapIrradianceCalculator() = default;

  void setHemisphereSampleDelta(float delta) {
    irradianceShader_.setHemisphereSampleDelta(delta);
  }

  // Draw onto the allocated cubemap from the given cubemap as the source.
  void multipassDraw(Texture source);

  Texture getIrradianceMap() { return cubemap_.asTexture(); }

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Framebuffer buffer_;
  Attachment cubemap_;
  CubemapIrradianceShader irradianceShader_;
};

}  // namespace qrk

#endif