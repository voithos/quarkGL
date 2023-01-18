#include <qrk/core.h>
#include <qrk/shader.h>
#include <qrk/shader_compiler.h>
#include <qrk/shader_loader.h>

namespace qrk {

Shader::Shader(const ShaderSource& vertexSource,
               const ShaderSource& fragmentSource) {
  ShaderCompiler compiler;
  compiler.loadAndCompileShader(vertexSource, ShaderType::VERTEX);
  compiler.loadAndCompileShader(fragmentSource, ShaderType::FRAGMENT);
  shaderProgram_ = compiler.linkShaderProgram();
}

Shader::Shader(const ShaderSource& vertexSource,
               const ShaderSource& fragmentSource,
               const ShaderSource& geometrySource) {
  ShaderCompiler compiler;
  compiler.loadAndCompileShader(vertexSource, ShaderType::VERTEX);
  compiler.loadAndCompileShader(fragmentSource, ShaderType::FRAGMENT);
  compiler.loadAndCompileShader(geometrySource, ShaderType::GEOMETRY);
  shaderProgram_ = compiler.linkShaderProgram();
}

int Shader::safeGetUniformLocation(const char* name) {
  int uniform = glGetUniformLocation(shaderProgram_, name);
  if (uniform == -1) {
    // TODO: Log a message; either uniform is invalid, or it got optimized away
    // by the shader.
    // printf("unknown uniform: %s\n", name);
  }
  return uniform;
}

void Shader::activate() { glUseProgram(shaderProgram_); }
void Shader::deactivate() { glUseProgram(0); }

// TODO: Is shared_ptr really the best approach here?
void Shader::addUniformSource(std::shared_ptr<UniformSource> source) {
  uniformSources_.push_back(source);
}

void Shader::updateUniforms() {
  // Update core uniforms.
  setFloat("qrk_time", qrk::time());

  for (auto uniformSource : uniformSources_) {
    uniformSource->updateUniforms(*this);
  }
}

void Shader::setBool(const char* name, bool value) {
  activate();
  glUniform1i(safeGetUniformLocation(name), static_cast<int>(value));
}

void Shader::setUInt(const char* name, unsigned int value) {
  activate();
  glUniform1ui(safeGetUniformLocation(name), value);
}

void Shader::setInt(const char* name, int value) {
  activate();
  glUniform1i(safeGetUniformLocation(name), value);
}

void Shader::setFloat(const char* name, float value) {
  activate();
  glUniform1f(safeGetUniformLocation(name), value);
}

void Shader::setVec3(const char* name, const glm::vec3& vector) {
  activate();
  glUniform3fv(safeGetUniformLocation(name), /*count=*/1,
               glm::value_ptr(vector));
}

void Shader::setVec3(const char* name, float v0, float v1, float v2) {
  activate();
  glUniform3f(safeGetUniformLocation(name), v0, v1, v2);
}

void Shader::setMat4(const char* name, const glm::mat4& matrix) {
  activate();
  glUniformMatrix4fv(safeGetUniformLocation(name), /*count=*/1,
                     /*transpose=*/GL_FALSE, glm::value_ptr(matrix));
}

ComputeShader::ComputeShader(const ShaderSource& computeSource) {
  ShaderCompiler compiler;
  compiler.loadAndCompileShader(computeSource, ShaderType::COMPUTE);
  shaderProgram_ = compiler.linkShaderProgram();
}

void ComputeShader::dispatchToTexture(Texture& texture) {
  activate();
  texture.bindToUnit(0, TextureBindType::IMAGE_TEXTURE);
  glDispatchCompute(texture.getWidth(), texture.getHeight(), 1);

  // Guard until writing is complete.
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

}  // namespace qrk
