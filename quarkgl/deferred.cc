#include <qrk/deferred.h>

namespace qrk {

DeferredGeometryPassShader::DeferredGeometryPassShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/deferred.vert"),
             ShaderPath("quarkgl/shaders/builtin/deferred.frag")) {}

GBuffer::GBuffer(int width, int height) : Framebuffer(width, height) {
  // Need to use a zero clear color, or else the G-Buffer won't work properly.
  setClearColor(glm::vec4(0.0f));
  // Create and attach all components of the G-Buffer.
  // Don't need to read from depth, so we attach as a renderbuffer.
  attachRenderbuffer(qrk::BufferType::DEPTH_AND_STENCIL);
  // Position and normal are stored as "HDR" colors for higher precision.
  // Alpha channels unused.
  positionBuffer_ = attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  normalBuffer_ = attachTexture(qrk::BufferType::COLOR_HDR_ALPHA);
  albedoSpecularBuffer_ = attachTexture(qrk::BufferType::COLOR_ALPHA);
  emissionBuffer_ = attachTexture(qrk::BufferType::COLOR_ALPHA);
}

unsigned int GBuffer::bindTexture(unsigned int nextTextureUnit,
                                  Shader& shader) {
  positionBuffer_.asTexture().bindToUnit(nextTextureUnit + 0);
  normalBuffer_.asTexture().bindToUnit(nextTextureUnit + 1);
  albedoSpecularBuffer_.asTexture().bindToUnit(nextTextureUnit + 2);
  emissionBuffer_.asTexture().bindToUnit(nextTextureUnit + 3);
  // Bind sampler uniforms.
  shader.setInt("gPosition", nextTextureUnit + 0);
  shader.setInt("gNormal", nextTextureUnit + 1);
  shader.setInt("gAlbedoSpecular", nextTextureUnit + 2);
  shader.setInt("gEmission", nextTextureUnit + 3);

  return nextTextureUnit + 4;
}

}  // namespace qrk
