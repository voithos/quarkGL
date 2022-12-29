#ifndef QUARKGL_BLOOM_H_
#define QUARKGL_BLOOM_H_

#include <qrk/framebuffer.h>
#include <qrk/mesh_primitives.h>
#include <qrk/shader.h>
#include <qrk/shader_primitives.h>
#include <qrk/texture_registry.h>

namespace qrk {

class BloomException : public QuarkException {
  using QuarkException::QuarkException;
};

class BloomBuffer : public Framebuffer, public TextureSource {
 public:
  BloomBuffer(int width, int height);
  explicit BloomBuffer(ImageSize size) : BloomBuffer(size.width, size.height) {}
  virtual ~BloomBuffer() = default;

  Texture getBloomMipChainTexture() {
    return bloomMipChainTexture_.asTexture();
  }

  int getNumMips() { return bloomMipChainTexture_.numMips; }

  // Limits sampling from any other mip other than the given mip. This is
  // important to avoid undefined behavior when drawing to a mip level while
  // sampling from another.
  void selectMip(int mipLevel);
  void deselectMip();

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Attachment bloomMipChainTexture_;
};

class BloomDownsampleShader : public ScreenShader {
 public:
  BloomDownsampleShader();

  void configureWith(BloomBuffer& buffer);
};

class BloomUpsampleShader : public ScreenShader {
 public:
  BloomUpsampleShader();

  void configureWith(BloomBuffer& buffer);
  // Sets the radius, in UV coordinates, for the upscaling kernel.
  void setFilterRadius(float filterRadius) {
    filterRadius_ = filterRadius;
    setFloat("qrk_filterRadius", filterRadius_);
  }
  float getFilterRadius() { return filterRadius_; }

 private:
  static constexpr float DEFAULT_FILTER_RADIUS = 0.005f;
  float filterRadius_ = DEFAULT_FILTER_RADIUS;
};

// A self contained bloom pass that uses a series of down/upsamples to perform
// the bloom effect.
class BloomPass : public TextureSource {
 public:
  BloomPass(int width, int height);
  explicit BloomPass(ImageSize size) : BloomPass(size.width, size.height) {}
  virtual ~BloomPass() = default;

  // Sets the radius, in UV coordinates, for the upscaling kernel.
  void setFilterRadius(float filterRadius) {
    upsampleShader_.setFilterRadius(filterRadius);
  }
  float getFilterRadius() { return upsampleShader_.getFilterRadius(); }

  // Performs the bloom based on the image from a source framebuffer. The
  // currently configured color component is blitted into the bloom buffer,
  // after which the bloom downsample/upsample calls are made.
  void multipassDraw(Framebuffer& sourceFb);

  int getNumMips() { return bloomBuffer_.getNumMips(); }
  void selectMip(int mipLevel) { bloomBuffer_.selectMip(mipLevel); }
  void deselectMip() { bloomBuffer_.deselectMip(); }

  Texture getOutput() { return bloomBuffer_.getBloomMipChainTexture(); }

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  ScreenQuadMesh screenQuad_;
  BloomBuffer bloomBuffer_;
  BloomDownsampleShader downsampleShader_;
  BloomUpsampleShader upsampleShader_;
};

}  // namespace qrk

#endif