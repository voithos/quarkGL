#ifndef QUARKGL_SHADER_H_
#define QUARKGL_SHADER_H_

#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <qrk/shared.h>

namespace qrk {

class ShaderException : public QuarkException {
  using QuarkException::QuarkException;
};

class Shader {
 private:
  unsigned int shaderProgram_;

 public:
  Shader(const char* vertexPath, const char* fragmentPath);
  unsigned int getProgramId() { return shaderProgram_; }

  void activate();
  void deactive();
  void setBool(const char* name, bool value);
  void setInt(const char* name, int value);
  void setFloat(const char* name, float value);
  void setVec3(const char* name, const glm::vec3& vector);
  void setVec3(const char* name, float v0, float v1, float v2);
  void setMat4(const char* name, const glm::mat4& matrix);
};
}  // namespace qrk

#endif
