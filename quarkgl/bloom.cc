#include <qrk/bloom.h>

namespace qrk {

BloomBuffer::BloomBuffer(int width, int height) : Framebuffer(width, height) {
  // Create and attach the bloom buffer. Don't need a depth buffer.
  qrk::TextureParams resampleParams = {
      .filtering = qrk::TextureFiltering::BILINEAR,
      .wrapMode = qrk::TextureWrapMode::CLAMP_TO_EDGE,
      .generateMips = qrk::MipGeneration::ALWAYS};
  bloomMipChainTexture_ =
      attachTexture(qrk::BufferType::COLOR_HDR_ALPHA, resampleParams);
}

void BloomBuffer::setSourceMip(int mipLevel) {
  if (mipLevel < 0 || mipLevel >= bloomMipChainTexture_.numMips) {
    throw BloomException("ERROR::BLOOM::SOURCE_MIP_OUT_OF_RANGE");
  }
  bloomMipChainTexture_.asTexture().setSamplerMipRange(mipLevel, mipLevel);
}

void BloomBuffer::unsetSourceMip() {
  bloomMipChainTexture_.asTexture().unsetSamplerMipRange();
}

unsigned int BloomBuffer::bindTexture(unsigned int nextTextureUnit,
                                      Shader& shader) {
  bloomMipChainTexture_.asTexture().bindToUnit(nextTextureUnit);
  // Bind sampler uniforms.
  shader.setInt("qrk_bloomMipChain", nextTextureUnit);

  return nextTextureUnit + 1;
}

BloomDownsampleShader::BloomDownsampleShader()
    : ScreenShader(
          ShaderPath("quarkgl/shaders/builtin/bloom_downsample.frag")) {}

void BloomDownsampleShader::configureWith(BloomBuffer& buffer) {
  // The bloom shader only needs a single texture, so we just bind it
  // directly.
  buffer.getBloomMipChainTexture().bindToUnit(0);
  setInt("qrk_bloomMipChain", 0);
}

}  // namespace qrk