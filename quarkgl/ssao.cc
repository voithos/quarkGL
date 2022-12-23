#include <qrk/ssao.h>
#include <qrk/utils.h>

#include <glm/gtx/norm.hpp>
#include <random>

namespace qrk {

SsaoShader::SsaoShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/screen_quad.vert"),
             ShaderPath("quarkgl/shaders/builtin/ssao.frag")) {}

SsaoKernel::SsaoKernel(float radius, float bias, int kernelSize,
                       int noiseTextureSideLength)
    : radius_(radius), bias_(bias) {
  regenerate(kernelSize, noiseTextureSideLength);
}

void SsaoKernel::regenerate(int kernelSize, int noiseTextureSideLength) {
  // Generate the kernel.
  kernel_.resize(kernelSize);
  for (int i = 0; i < kernelSize; ++i) {
    // Generate a hemisphere sample, with the normal vector pointing in the
    // positive Z direction.

    // First we generate a vector along the sample space.
    glm::vec3 sample(rand_.next() * 2.0f - 1.0f, rand_.next() * 2.0f - 1.0f,
                     rand_.next());
    // Reject samples outside the sphere, to avoid over-sampling the corners.
    if (glm::length2(sample) >= 1.0f) {
      --i;
      continue;
    }

    // Use the vector to generate a point in the hemisphere.
    sample = glm::normalize(sample) * rand_.next();

    // At this point we have a random point sampled in the hemisphere, but we
    // want to sample more points closer to the actual fragment, so we scale the
    // result.
    float scale = static_cast<float>(i) / kernelSize;
    scale = lerp(0.1f, 1.0f, scale * scale);
    sample *= scale;

    kernel_[i] = sample;
  }

  // Generate the noise texture used to rotate the kernel.
  std::vector<glm::vec3> noiseData;
  int noiseDataSize = noiseTextureSideLength * noiseTextureSideLength;
  noiseData.resize(noiseDataSize);
  for (int i = 0; i < noiseDataSize; ++i) {
    // Generate a vector on the XY normal plane which we'll use to randomly
    // "tilt" the sample hemisphere in the shader.
    glm::vec3 noise(rand_.next() * 2.0f - 1.0f, rand_.next() * 2.0f - 1.0f,
                    0.0f);
    noiseData[i] = noise;
  }

  // Generate the texture. We don't want texture filtering, and we must enable
  // repeat wrap mode so that it properly tiles over the screen.
  TextureParams params = {
      .filtering = TextureFiltering::NEAREST,
      .wrapMode = TextureWrapMode::REPEAT,
  };
  noiseTexture_ =
      Texture::createFromData(noiseTextureSideLength, noiseTextureSideLength,
                              /*internalFormat=*/GL_RGB16F, noiseData, params);
}

void SsaoKernel::updateUniforms(Shader& shader) {
  shader.setFloat("qrk_ssaoSampleRadius", radius_);
  shader.setFloat("qrk_ssaoSampleBias", bias_);
  shader.setInt("qrk_ssaoKernelSize", kernel_.size());
  for (unsigned int i = 0; i < kernel_.size(); ++i) {
    shader.setVec3("qrk_ssaoKernel[" + std::to_string(i) + "]", kernel_[i]);
  }
}

unsigned int SsaoKernel::bindTexture(unsigned int nextTextureUnit,
                                     Shader& shader) {
  noiseTexture_.bindToUnit(nextTextureUnit);
  // Bind sampler uniforms.
  shader.setInt("qrk_ssaoNoise", nextTextureUnit);

  return nextTextureUnit + 1;
}

SsaoBuffer::SsaoBuffer(int width, int height) : Framebuffer(width, height) {
  // Make sure we're clearing properly.
  setClearColor(glm::vec4(0.0f));
  // Create and attach the SSAO buffer. Don't need a depth buffer.
  ssaoBuffer_ = attachTexture(qrk::BufferType::GRAYSCALE);
}

unsigned int SsaoBuffer::bindTexture(unsigned int nextTextureUnit,
                                     Shader& shader) {
  ssaoBuffer_.asTexture().bindToUnit(nextTextureUnit);
  // Bind sampler uniforms.
  shader.setInt("qrk_ssao", nextTextureUnit);

  return nextTextureUnit + 1;
}

SsaoBlurShader::SsaoBlurShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/screen_quad.vert"),
             ShaderPath("quarkgl/shaders/builtin/ssao_blur.frag")) {}

void SsaoBlurShader::configureWith(SsaoKernel& kernel, SsaoBuffer& buffer) {
  setInt("qrk_ssaoNoiseTextureSideLength", kernel.getNoiseTextureSideLength());

  // The blur shader only needs a single texture, so we just bind it directly.
  buffer.getSsaoTexture().bindToUnit(0);
  setInt("qrk_ssao", 0);
}

}  // namespace qrk
