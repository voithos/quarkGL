#include <glad/glad.h>
#include <qrk/shader_primitives.h>

#include <cstring>

namespace qrk {

SkyboxShader::SkyboxShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/skybox.vert"),
             ShaderPath("quarkgl/shaders/builtin/skybox.frag")) {}

void SkyboxShader::activate() {
  // The shader always outputs a depth of 1.0 (the max depth) for skybox
  // fragments, so we have to switch the depth function to LEQUAL in order for
  // them to render at all.
  glDepthFunc(GL_LEQUAL);
  Shader::activate();
}

void SkyboxShader::deactivate() {
  Shader::deactivate();
  glDepthFunc(GL_LESS);
}

void SkyboxShader::setMat4(const char* name, const glm::mat4& matrix) {
  // Special case the "view" transform.
  if (strcmp(name, "view") == 0) {
    // We drop the translation (by just converting to a mat3 and then back)
    // since the skybox needs to always follow the camera.
    Shader::setMat4(name, glm::mat4(glm::mat3(matrix)));
    return;
  }
  Shader::setMat4(name, matrix);
}

ScreenShader::ScreenShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/screen_quad.vert"),
             ShaderPath("quarkgl/shaders/builtin/screen_quad.frag")) {}

ScreenShader::ScreenShader(const ShaderSource& fragmentSource)
    : Shader(ShaderPath("quarkgl/shaders/builtin/screen_quad.vert"),
             fragmentSource) {}

ScreenLodShader::ScreenLodShader()
    : ScreenShader(ShaderPath("quarkgl/shaders/builtin/screen_quad_lod.frag")) {
}

ShadowMapShader::ShadowMapShader()
    : Shader(ShaderPath("quarkgl/shaders/builtin/shadow_map.vert"),
             ShaderPath("quarkgl/shaders/builtin/shadow_map.frag")) {}

}  // namespace qrk
