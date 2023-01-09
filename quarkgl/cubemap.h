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
  CubeMesh cube_;
  Framebuffer buffer_;
  Attachment cubemap_;
  EquirectCubemapShader equirectCubemapShader_;
};

}  // namespace qrk

#endif