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

void BloomBuffer::selectMip(int mipLevel) {
  if (mipLevel < 0 || mipLevel >= bloomMipChainTexture_.numMips) {
    throw BloomException("ERROR::BLOOM::SOURCE_MIP_OUT_OF_RANGE");
  }
  bloomMipChainTexture_.asTexture().setSamplerMipRange(mipLevel, mipLevel);
}

void BloomBuffer::deselectMip() {
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

BloomUpsampleShader::BloomUpsampleShader()
    : ScreenShader(ShaderPath("quarkgl/shaders/builtin/bloom_upsample.frag")) {
  setFilterRadius(filterRadius_);
}

void BloomUpsampleShader::configureWith(BloomBuffer& buffer) {
  // The bloom shader only needs a single texture, so we just bind it
  // directly.
  buffer.getBloomMipChainTexture().bindToUnit(0);
  setInt("qrk_bloomMipChain", 0);
}

BloomPass::BloomPass(int width, int height) : bloomBuffer_(width, height) {}

void BloomPass::multipassDraw(Framebuffer& sourceFb) {
  // Copy to mip level 0.
  bloomBuffer_.activate(0);
  sourceFb.blit(bloomBuffer_, GL_COLOR_BUFFER_BIT);

  int numMips = bloomBuffer_.getNumMips();

  // Perform the downsampling across the mip chain.
  downsampleShader_.configureWith(bloomBuffer_);
  for (int destMip = 1; destMip < numMips; ++destMip) {
    bloomBuffer_.activate(destMip);
    int sourceMip = destMip - 1;
    bloomBuffer_.selectMip(sourceMip);
    screenQuad_.draw(downsampleShader_);
  }

  // Perform the upsampling, starting with the second-to-last mip. We enable
  // additive blending to avoid having to render into a separate texture.
  bloomBuffer_.enableAdditiveBlending();
  upsampleShader_.configureWith(bloomBuffer_);
  for (int destMip = numMips - 2; destMip >= 0; --destMip) {
    bloomBuffer_.activate(destMip);
    int sourceMip = destMip + 1;
    bloomBuffer_.selectMip(sourceMip);
    screenQuad_.draw(upsampleShader_);
  }

  bloomBuffer_.deselectMip();
  bloomBuffer_.disableAdditiveBlending();
  bloomBuffer_.deactivate();
}

unsigned int BloomPass::bindTexture(unsigned int nextTextureUnit,
                                    Shader& shader) {
  getOutput().bindToUnit(nextTextureUnit);
  // Bind sampler uniforms.
  shader.setInt("qrk_bloom", nextTextureUnit);

  return nextTextureUnit + 1;
}

}  // namespace qrk