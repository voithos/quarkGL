#ifndef QUARKGL_IBL_H_
#define QUARKGL_IBL_H_

#include <qrk/cubemap.h>
#include <qrk/framebuffer.h>
#include <qrk/mesh_primitives.h>
#include <qrk/shader.h>
#include <qrk/shader_primitives.h>
#include <qrk/texture_registry.h>

namespace qrk {

class IblException : public QuarkException {
  using QuarkException::QuarkException;
};

class CubemapIrradianceShader : public Shader {
 public:
  CubemapIrradianceShader();

  float getHemisphereSampleDelta() const { return hemisphereSampleDelta_; }
  void setHemisphereSampleDelta(float delta);

 private:
  float hemisphereSampleDelta_ = 0.025f;
};

// Calculates a diffuse irradiance map based on a given HDR cubemap.
// Note that since each pixel in the irradiance map is uniformly sampled from an
// entire hemisphere, fine details are not preserved - thus the irradiance map
// need not be very large (32x32 can suffice), and the texture's bilinear
// filtering can fill in the blanks.
class CubemapIrradianceCalculator : public TextureSource {
 public:
  CubemapIrradianceCalculator(int width = 32, int height = 32);
  explicit CubemapIrradianceCalculator(ImageSize size)
      : CubemapIrradianceCalculator(size.width, size.height) {}
  virtual ~CubemapIrradianceCalculator() = default;

  float getHemisphereSampleDelta() const {
    return irradianceShader_.getHemisphereSampleDelta();
  }
  void setHemisphereSampleDelta(float delta) {
    irradianceShader_.setHemisphereSampleDelta(delta);
  }

  // Draw onto the allocated cubemap from the given cubemap as the source.
  void multipassDraw(Texture source);

  Texture getIrradianceMap() { return cubemap_.asTexture(); }

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Framebuffer buffer_;
  Attachment cubemap_;
  CubemapIrradianceShader irradianceShader_;
};

class GGXPrefilterShader : public Shader {
 public:
  GGXPrefilterShader();

  unsigned int getNumSamples() const { return numSamples_; }
  void setNumSamples(unsigned int samples);

  void setRoughness(float roughness);

 private:
  unsigned int numSamples_ = 1024;
};

// Calculates the prefiltered env map based on the GGX microfacet model. The map
// contains multiple mip level, which each mip level representing a different
// material roughness (mip0 -> roughness 0).
class GGXPrefilteredEnvMapCalculator : public TextureSource {
 public:
  GGXPrefilteredEnvMapCalculator(int width, int height, int maxNumMips = -1);
  explicit GGXPrefilteredEnvMapCalculator(ImageSize size, int maxNumMips = -1)
      : GGXPrefilteredEnvMapCalculator(size.width, size.height, maxNumMips) {}
  virtual ~GGXPrefilteredEnvMapCalculator() = default;

  unsigned int getNumSamples() const { return shader_.getNumSamples(); }
  void setNumSamples(unsigned int samples) { shader_.setNumSamples(samples); }

  // Draw onto the allocated prefiltering cubemap from the given cubemap as the
  // source. The cubemap should ideally have mip levels in order to avoid
  // hotspot artifacts.
  void multipassDraw(Texture source);

  Texture getPrefilteredEnvMap() { return cubemap_.asTexture(); }

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Framebuffer buffer_;
  Attachment cubemap_;
  GGXPrefilterShader shader_;
};

class GGXBrdfIntegrationShader : public ScreenShader {
 public:
  GGXBrdfIntegrationShader();

  unsigned int getNumSamples() const { return numSamples_; }
  void setNumSamples(unsigned int samples);

 private:
  unsigned int numSamples_ = 1024;
};

// Calculates an integration map for the GGX BRDF, parameterized over roughness
// and NdotV.
class GGXBrdfIntegrationCalculator : public TextureSource {
 public:
  GGXBrdfIntegrationCalculator(int width, int height);
  explicit GGXBrdfIntegrationCalculator(ImageSize size)
      : GGXBrdfIntegrationCalculator(size.width, size.height) {}
  virtual ~GGXBrdfIntegrationCalculator() = default;

  unsigned int getNumSamples() const { return shader_.getNumSamples(); }
  void setNumSamples(unsigned int samples) { shader_.setNumSamples(samples); }

  // Draw onto the allocated BRDF integration texture. This is solely a function
  // of the BRDF and does not require any source data.
  void draw();

  Texture getBrdfIntegrationMap() { return integrationMap_.asTexture(); }

  unsigned int bindTexture(unsigned int nextTextureUnit,
                           Shader& shader) override;

 private:
  Framebuffer buffer_;
  Attachment integrationMap_;
  ScreenQuadMesh screenQuad_;
  GGXBrdfIntegrationShader shader_;
};

}  // namespace qrk

#endif