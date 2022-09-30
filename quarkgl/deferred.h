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
  explicit GBuffer(ScreenSize size) : GBuffer(size.width, size.height) {}
  virtual ~GBuffer() = default;

  Texture getPositionTexture() { return positionBuffer_.asTexture(); }
  Texture getNormalTexture() { return normalBuffer_.asTexture(); }
  Texture getAlbedoSpecularTexture() {
    return albedoSpecularBuffer_.asTexture();
  }
  Texture getEmissionTexture() { return emissionBuffer_.asTexture(); }

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Attachment positionBuffer_;
  Attachment normalBuffer_;
  // RGB used for albedo, A used for specularity.
  Attachment albedoSpecularBuffer_;
  Attachment emissionBuffer_;
};

}  // namespace qrk

#endif
