#ifndef QUARKGL_SSAO_H_
#define QUARKGL_SSAO_H_

#include <qrk/exceptions.h>
#include <qrk/framebuffer.h>
#include <qrk/random.h>
#include <qrk/shader.h>
#include <qrk/texture.h>
#include <qrk/texture_registry.h>

#include <glm/glm.hpp>
#include <vector>

namespace qrk {

class SsaoException : public QuarkException {
  using QuarkException::QuarkException;
};

// A shader that calculates SSAO based on a G-Buffer and SSAO kernel.
class SsaoShader : public Shader {
 public:
  SsaoShader();
};

// A sample kernel for use in screen space ambient occlusion. Uses a hemisphere
// sampling method and a noise texture.
class SsaoKernel : public UniformSource, public TextureSource {
 public:
  SsaoKernel(float radius = 0.5, float bias = 0.025, int kernelSize = 64,
             int noiseTextureSideLength = 4);
  int getSize() { return kernel_.size(); }
  int getNoiseTextureSideLength() { return noiseTexture_.getWidth(); }
  float getRadius() { return radius_; }
  void setRadius(float radius) { radius_ = radius; }
  float getBias() { return bias_; }
  void setBias(float bias) { bias_ = bias; }

  // Binds kernel uniforms.
  void updateUniforms(Shader& shader) override;

  // Binds the noise texture.
  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  void regenerate(int kernelSize, int noiseTextureSideLength);

  float radius_;
  float bias_;
  std::vector<glm::vec3> kernel_;
  Texture noiseTexture_;
  UniformRandom rand_;
};

class SsaoBuffer : public Framebuffer, public TextureSource {
 public:
  SsaoBuffer(int width, int height);
  explicit SsaoBuffer(ImageSize size) : SsaoBuffer(size.width, size.height) {}
  virtual ~SsaoBuffer() = default;

  Texture getSsaoTexture() { return ssaoBuffer_.asTexture(); }

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Attachment ssaoBuffer_;
};

// A simple shader that blurs a precomputed SSAO buffer.
class SsaoBlurShader : public Shader {
 public:
  SsaoBlurShader();

  // Configures uniforms for the blur to run correctly for the given kernel,
  // using the given buffer as the source image. After this, you can draw onto
  // another separate SsaoBuffer to get the blurred result.
  void configureWith(SsaoKernel& kernel, SsaoBuffer& buffer);
};

}  // namespace qrk

#endif