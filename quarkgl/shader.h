#ifndef QUARKGL_SHADER_H_
#define QUARKGL_SHADER_H_

#include <memory>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <qrk/shared.h>

namespace qrk {

class Shader;

class ShaderException : public QuarkException {
  using QuarkException::QuarkException;
};

class UniformSource {
 public:
  virtual void updateUniforms(Shader& shader) = 0;
};

class Shader {
 private:
  unsigned int shaderProgram_;
  std::vector<std::shared_ptr<UniformSource>> uniformSources_;

  void compileShaderProgram(unsigned int vertexShader,
                            unsigned int fragmentShader,
                            unsigned int geometryShader);
  unsigned int loadAndCompileShader(const ShaderSource& shaderSource,
                                    const ShaderType type);
  unsigned int compileShader(const char* shaderSource, const ShaderType type);

  int safeGetUniformLocation(const char* name);

 public:
  Shader(const ShaderSource& vertexSource, const ShaderSource& fragmentSource);
  Shader(const ShaderSource& vertexSource, const ShaderSource& fragmentSource,
         const ShaderSource& geometrySource);
  unsigned int getProgramId() { return shaderProgram_; }

  void activate();
  void deactive();

  void addUniformSource(std::shared_ptr<UniformSource> source);
  void updateUniforms();

  void setBool(const char* name, bool value);
  void setBool(std::string name, bool value) { setBool(name.c_str(), value); }
  void setInt(const char* name, int value);
  void setInt(std::string name, int value) { setInt(name.c_str(), value); }
  void setFloat(const char* name, float value);
  void setFloat(std::string name, float value) {
    setFloat(name.c_str(), value);
  }
  void setVec3(const char* name, const glm::vec3& vector);
  void setVec3(std::string name, const glm::vec3& vector) {
    setVec3(name.c_str(), vector);
  }
  void setVec3(const char* name, float v0, float v1, float v2);
  void setVec3(std::string name, float v0, float v1, float v2) {
    setVec3(name.c_str(), v0, v1, v2);
  }
  void setMat4(const char* name, const glm::mat4& matrix);
  void setMat4(std::string name, const glm::mat4& matrix) {
    setMat4(name.c_str(), matrix);
  }
};
}  // namespace qrk

#endif
