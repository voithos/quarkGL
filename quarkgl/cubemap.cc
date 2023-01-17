#include <qrk/cubemap.h>

namespace qrk {

void CubemapRenderHelper::multipassDraw(Shader& shader,
                                        TextureRegistry* textureRegistry) {
  // Set projection to a 90-degree, 1:1 aspect ratio in order to render a single
  // face of the cube.
  shader.setMat4("projection", glm::perspective(glm::radians(90.0f),
                                                /*aspect=*/1.0f, 0.1f, 10.0f));

  // TODO: Why are the up vectors negative?
  glm::mat4 faceViews[] = {
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
      glm::lookAt(/*eye=*/glm::vec3(0.0f, 0.0f, 0.0f),
                  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

  for (int cubemapFace = 0; cubemapFace < 6; ++cubemapFace) {
    buffer_->activate(targetMip_, cubemapFace);
    buffer_->clear();

    shader.setMat4("view", faceViews[cubemapFace]);
    cube_.draw(shader, textureRegistry);
  }

  buffer_->deactivate();
}

EquirectCubemapShader::EquirectCubemapShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/cubemap.vert"),
             ShaderPath("quarkgl/shaders/builtin/equirect_cubemap.frag")) {}

EquirectCubemapConverter::EquirectCubemapConverter(int width, int height,
                                                   bool generateMips)
    : buffer_(width, height), generateMips_(generateMips) {
  // Optionally allocate memory for mips if requested.
  TextureParams params = {
      .filtering = generateMips ? TextureFiltering::TRILINEAR
                                : TextureFiltering::BILINEAR,
      .wrapMode = TextureWrapMode::CLAMP_TO_EDGE,
      .generateMips =
          generateMips ? MipGeneration::ALWAYS : MipGeneration::NEVER,
  };
  cubemap_ = buffer_.attachTexture(BufferType::COLOR_CUBEMAP_HDR, params);
}

void EquirectCubemapConverter::multipassDraw(Texture source) {
  // Set up the source.
  source.bindToUnit(0, TextureBindType::TEXTURE_2D);
  equirectCubemapShader_.setInt("qrk_equirectMap", 0);

  CubemapRenderHelper renderHelper(&buffer_);
  renderHelper.multipassDraw(equirectCubemapShader_);

  if (generateMips_) {
    // Generate mips after having rendered to the cubemap.
    cubemap_.asTexture().generateMips();
  }
}

unsigned int EquirectCubemapConverter::bindTexture(unsigned int nextTextureUnit,
                                                   Shader& shader) {
  cubemap_.asTexture().bindToUnit(nextTextureUnit, TextureBindType::CUBEMAP);
  // Bind sampler uniforms.
  shader.setInt("qrk_cubemap", nextTextureUnit);

  return nextTextureUnit + 1;
}

}  // namespace qrk
