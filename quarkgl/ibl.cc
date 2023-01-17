#include "ibl.h"

#include <qrk/ibl.h>

namespace qrk {

CubemapIrradianceShader::CubemapIrradianceShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/cubemap.vert"),
             ShaderPath("quarkgl/shaders/builtin/irradiance_cubemap.frag")) {
  // Set defaults.
  setHemisphereSampleDelta(hemisphereSampleDelta_);
}

void CubemapIrradianceShader::setHemisphereSampleDelta(float delta) {
  hemisphereSampleDelta_ = delta;
  setFloat("qrk_hemisphereSampleDelta", hemisphereSampleDelta_);
}

CubemapIrradianceCalculator::CubemapIrradianceCalculator(int width, int height)
    : buffer_(width, height) {
  cubemap_ = buffer_.attachTexture(BufferType::COLOR_CUBEMAP_HDR);
}

void CubemapIrradianceCalculator::multipassDraw(Texture source) {
  // Set up the source.
  source.bindToUnit(0, TextureBindType::CUBEMAP);
  irradianceShader_.setInt("qrk_environmentMap", 0);

  CubemapRenderHelper renderHelper(&buffer_);
  renderHelper.multipassDraw(irradianceShader_);
}

unsigned int CubemapIrradianceCalculator::bindTexture(
    unsigned int nextTextureUnit, Shader& shader) {
  cubemap_.asTexture().bindToUnit(nextTextureUnit, TextureBindType::CUBEMAP);
  // Bind sampler uniforms.
  shader.setInt("qrk_irradianceMap", nextTextureUnit);

  return nextTextureUnit + 1;
}

GGXPrefilterShader::GGXPrefilterShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/cubemap.vert"),
             ShaderPath("quarkgl/shaders/builtin/ggx_prefilter_cubemap.frag")) {
  setNumSamples(numSamples_);
}

void GGXPrefilterShader::setNumSamples(unsigned int samples) {
  numSamples_ = samples;
  setUInt("qrk_numSamples", numSamples_);
}

void GGXPrefilterShader::setRoughness(float roughness) {
  setFloat("qrk_roughness", roughness);
}

qrk::GGXPrefilteredEnvMapCalculator::GGXPrefilteredEnvMapCalculator(
    int width, int height, int maxNumMips)
    : buffer_(width, height) {
  TextureParams textureParams = {
      // Need trilinear filtering in order to make use of mip levels.
      .filtering = TextureFiltering::TRILINEAR,
      // Clamp to the edge so we don't oversample.
      .wrapMode = TextureWrapMode::CLAMP_TO_EDGE,
      // Force mip generation. We can optionally just use a smaller part of the
      // mip chain.
      .generateMips = MipGeneration::ALWAYS,
      .maxNumMips = maxNumMips,
  };
  cubemap_ =
      buffer_.attachTexture(BufferType::COLOR_CUBEMAP_HDR, textureParams);
}

void GGXPrefilteredEnvMapCalculator::multipassDraw(Texture source) {
  // Set up the source.
  source.bindToUnit(0, TextureBindType::CUBEMAP);
  shader_.setInt("qrk_environmentMap", 0);

  CubemapRenderHelper renderHelper(&buffer_);
  for (int mip = 0; mip < cubemap_.numMips; ++mip) {
    renderHelper.setTargetMip(mip);
    // Go through roughness from [0..1].
    float roughness = static_cast<float>(mip) / (cubemap_.numMips - 1);
    shader_.setRoughness(roughness);
    renderHelper.multipassDraw(shader_);
  }
}

unsigned int GGXPrefilteredEnvMapCalculator::bindTexture(
    unsigned int nextTextureUnit, Shader& shader) {
  cubemap_.asTexture().bindToUnit(nextTextureUnit, TextureBindType::CUBEMAP);
  // Bind sampler uniforms.
  shader.setInt("qrk_ggxPrefilteredEnvMap", nextTextureUnit);

  return nextTextureUnit + 1;
}

}  // namespace qrk