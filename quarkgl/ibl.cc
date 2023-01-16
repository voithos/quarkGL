#include <qrk/ibl.h>

namespace qrk {

CubemapIrradianceShader::CubemapIrradianceShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/cubemap.vert"),
             ShaderPath("quarkgl/shaders/builtin/irradiance_cubemap.frag")) {
  // Set defaults.
  setHemisphereSampleDelta(0.025f);
}

void CubemapIrradianceShader::setHemisphereSampleDelta(float delta) {
  setFloat("qrk_hemisphereSampleDelta", delta);
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

}  // namespace qrk