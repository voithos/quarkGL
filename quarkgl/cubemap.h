#ifndef QUARKGL_CUBEMAP_H_
#define QUARKGL_CUBEMAP_H_

#include <qrk/framebuffer.h>
#include <qrk/mesh_primitives.h>
#include <qrk/shader.h>
#include <qrk/shader_primitives.h>
#include <qrk/texture_registry.h>

namespace qrk {

class CubemapException : public QuarkException {
  using QuarkException::QuarkException;
};

// A helper for rendering to a cubemap texture in a framebuffer. The passed-in
// framebuffer must outlive the life of this helper.
class CubemapRenderHelper {
 public:
  CubemapRenderHelper(Framebuffer* buffer) : buffer_(buffer) {}

  // Draws with the given shader to each face of the cubemap. This results in 6
  // different draw calls. Shader should be prepared (i.e. necessary textures
  // should either be bound or be in the registry, uniforms should be set, etc).
  void multipassDraw(Shader& shader,
                     TextureRegistry* textureRegistry = nullptr);

 private:
  Framebuffer* buffer_;
  CubeMesh cube_;
};

class EquirectCubemapShader : public Shader {
 public:
  EquirectCubemapShader();
};

// Converts an equirect texture to a cubemap.
class EquirectCubemapConverter : public TextureSource {
 public:
  EquirectCubemapConverter(int width, int height);
  explicit EquirectCubemapConverter(ImageSize size)
      : EquirectCubemapConverter(size.width, size.height) {}
  virtual ~EquirectCubemapConverter() = default;

  // Draw onto the allocated cubemap from the given texture as the source.
  void multipassDraw(Texture source);

  Texture getCubemap() { return cubemap_.asTexture(); }

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Framebuffer buffer_;
  Attachment cubemap_;
  EquirectCubemapShader equirectCubemapShader_;
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
  CubemapIrradianceCalculator(int width, int height);
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