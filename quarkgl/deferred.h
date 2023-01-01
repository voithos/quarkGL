#ifndef QUARKGL_DEFERRED_H_
#define QUARKGL_DEFERRED_H_

#include <qrk/exceptions.h>
#include <qrk/framebuffer.h>
#include <qrk/shader.h>
#include <qrk/texture.h>
#include <qrk/texture_registry.h>

#include <glm/glm.hpp>

namespace qrk {

class DeferredShadingException : public QuarkException {
  using QuarkException::QuarkException;
};

class DeferredGeometryPassShader : public Shader {
 public:
  DeferredGeometryPassShader();
};

class GBuffer : public Framebuffer, public TextureSource {
 public:
  GBuffer(int width, int height);
  explicit GBuffer(ImageSize size) : GBuffer(size.width, size.height) {}
  virtual ~GBuffer() = default;

  Texture getPositionAOTexture() { return positionAOBuffer_.asTexture(); }
  Texture getNormalRoughnessTexture() {
    return normalRoughnessBuffer_.asTexture();
  }
  Texture getAlbedoMetallicTexture() {
    return albedoMetallicBuffer_.asTexture();
  }
  Texture getEmissionTexture() { return emissionBuffer_.asTexture(); }

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  // RGB used for position, alpha used for AO.
  Attachment positionAOBuffer_;
  // RGB used for normal, alpha used for roughness.
  Attachment normalRoughnessBuffer_;
  // RGB used for albedo, alpha used for metallic.
  Attachment albedoMetallicBuffer_;
  // RGB used for emission color, alpha channel unused.
  Attachment emissionBuffer_;
};

}  // namespace qrk

#endif
